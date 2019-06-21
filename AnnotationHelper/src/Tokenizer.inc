<?php


namespace Inte\Annotation\Helper;


use ReflectionClass;

class Tokenizer {
  public function __construct(ReflectionClass $class) {
    $this->class_ = $class;
    $this->tokens_ = token_get_all(
      file_get_contents($this->class_->getFileName()));
  }

  static public function createTokenizer(ReflectionClass $class):self {
    if ($class->isInterface()) {
      return new InterfaceTokenizer($class);
    }
    return new self($class);
  }

  public function directlyImplementInterface(ReflectionClass $interface):bool {
    $alias = $this->getAliasNames($interface);
    $implements = $this->getAllImplementAlias();

    return count(array_intersect($alias, $implements));
  }

  public function getAliasNames(ReflectionClass $interfaceOrClass):array {
    $alias = [];

    // 1  interface 的命名空间是class的子命名空间, 或者同命名空间
    if ($this->class_->getNamespaceName() !== null
      && $this->class_->getNamespaceName() !== "") {

      $pos = strpos($interfaceOrClass->getName()
        , $this->class_->getNamespaceName());
      if ($pos === 0) {
        $alias[] = substr($interfaceOrClass->getName()
          , strlen($this->class_->getNamespaceName()) + 1/* 去掉\ */);
      }
    }

    // 2 使用 use ... [as] ...  引入的, 包括只是引入了子空间
    // eg: use A\C as CC;  xxxx CC\D  => A\C\D

    $process = -1;
    $name = ["", $interfaceOrClass->getShortName()]; // use=>as
    foreach ($this->tokens_ as $token) {
      if (is_array($token) && $token[0] === T_USE) {
        $process = 0;
        continue;
      }

      if ($process == 0 && is_array($token) && $token[0] === T_AS) {
        $process = 1;
        continue;
      }

      if ($token === ";" && $process != -1) {
        $pos = strpos($interfaceOrClass->getName(), $name[0]);
        if ($pos === 0) {
          $pos = strlen($name[0]);
          $left = substr($interfaceOrClass->getName(), $pos);
          $alias[] = $name[1] . $left;
        }

        $process = -1;
        $name = ["", $interfaceOrClass->getShortName()];
        continue;
      }

      if ($process == 0 && $token[0] !== T_WHITESPACE) {
        $name[0] = $name[0].$token[1];
        continue;
      }

      if ($process == 1 && $token[0] !== T_WHITESPACE) {
        $name[1] = $token[1];
      }
    }

    // 3 全限定名
    $alias[] = "\\" . $interfaceOrClass->getName();

    return $alias;
  }

  protected function getInterfaceImplementsKeywordToken():int {
    return T_IMPLEMENTS;
  }

  protected function getDefineKeywordToken():int {
    return T_CLASS;
  }

  public function getAllImplements():array {
    $ret = [];
    $implAlias = $this->getAllImplementAlias();
    $uses = $this->getAllUseMap();
    foreach ($implAlias as $impl) {
      // 全限定名
      if ($impl[0] === '\\') {
        $ret[] = substr($impl, 1);
        continue;
      }

      $found = false;
      foreach ($uses as $as=>$use) {
        // use A\C as CC ,  CC\D => A\C\D
        // use A\C, C\D => A\C\D
        // use A\C\D, D => A\C\D
        $pos = strpos($impl, $as);
        if ($pos === 0) {
          $ret[] = $use . substr($impl, strlen($as));
          $found = true;
        }
      }
      if ($found) {
        continue;
      }

      $ret[] = $this->class_->getNamespaceName() . "\\" . $impl;
    }

    return $ret;
  }

  private function getAllImplementAlias():array {
    $implements = [];

    $thisClass = 0;
    $nowImpl = "";
    foreach ($this->tokens_ as $token) {
      if ($thisClass ==0 && is_array($token)
        && $token[0] === $this->getDefineKeywordToken()) {
        $thisClass = 1;
        continue;
      }

      if ($thisClass == 1 && is_array($token) && $token[0] === T_STRING) {
        if ($token[1] === $this->class_->getShortName()) {
          $thisClass = 2;
        } else {
          $thisClass = 0;
        }
        continue;
      }

      if ($thisClass != 0 && $token === "{") {
        if ($thisClass === 3) {
          $implements[] = $nowImpl;
          $nowImpl = "";
        }
        break;
      }

      if ($thisClass == 2 && is_array($token) && $token[0]
          === $this->getInterfaceImplementsKeywordToken()) {

        $thisClass = 3;
        continue;
      }

      if ($thisClass == 3 && $token === ',' && $nowImpl !== "") {
        $implements[] = $nowImpl;
        $nowImpl = "";
      }

      if ($thisClass == 3 && is_array($token)
        && ($token[0] === T_STRING || $token[0] === T_NS_SEPARATOR)) {
        $nowImpl = $nowImpl . $token[1];
      }
    }

    return $implements;
  }

  /**
   *
   * 类型 string|bool|int|float|array|object|null|callback|mixed|resource|void
   *  |<类>|[]标示的数组
   *
   *  '@var' 后面定义的类型，就是返回值，没有此注释就返回null，| 可以分隔多个类型
   *
   * @throws \ReflectionException
   */
  public function getPropertyTypeName(string $propertyName):array {

    $refProperty = $this->class_->getProperty($propertyName);
    $doc = $refProperty->getDocComment();
    $matched = preg_match(
      '/^\s*\*\s*@var\s+?'
      .'(((\\\\??[a-zA-Z_][a-zA-Z0-9_]*?)+?(\[\])?\|?)+)'
      .'\s+?/mU'
      , $doc, $matches);

    $ret = [];

    if ($matched === false || $matched === 0
      || count($matches) < 2 || $matches[1] === null) {
      return $ret;
    }

    $primitive = [
      'string'=>'string',
      'bool'=>'bool',
      'int'=>'int',
      'float'=>'float',
      'array'=>'array',
      'object'=>'object',
      'null'=>'null',
      'callback'=>'callback',
      'mixed'=>'mixed',
      'resource'=>'resource',
      'integer'=>'int',
      'boolean'=>'bool',
      'double'=>'float',
      'void'=>'void'
      ];

    $types = explode('|', $matches[1]);

    foreach ($types as $type) {
      $suffix = "";
      if (strlen($type) > 2 && $type[-2]=='['&&$type[-1]==']') {
        $suffix = '[]';
        $type = substr($type, 0, -2);
      }

      if (@$primitive[$type] !== null) {
        $ret[] = $primitive[$type] . $suffix;
        continue;
      }

      if (strpos($type, '\\') === 0) {
        $ret[] = substr($type, 1) . $suffix;
        continue;
      }

      $uses = $this->getAllUseMap();
      $fullNm = @$uses[$type];
      if ($fullNm !== null) {
        $ret[] = $fullNm . $suffix;
        continue;
      }

      $ret[] = $this->class_->getNamespaceName() . '\\' . $type . $suffix;
    }

    return $ret;
  }

  /**
   * @return [as=>fullName] array
   */
  public function getAllUseMap():array {
    $process = -1;
    $name = ["", ""]; // use=>as
    $ret = [];

    foreach ($this->tokens_ as $token) {
      if (is_array($token) && $token[0] === T_USE) {
        $process = 0;
        continue;
      }

      if ($process == 0 && is_array($token) && $token[0] === T_AS) {
        $process = 1;
        continue;
      }

      if ($token === ";" && $process != -1) {
        $ret[$name[1]] = $name[0];

        $process = -1;
        $name = ["", ""];
        continue;
      }

      if ($process == 0 && $token[0] !== T_WHITESPACE) {
        if ($token[0] === T_STRING) {
          $name[1] = $token[1];
        }
        $name[0] .= $token[1];
        continue;
      }

      if ($process == 1 && $token[0] !== T_WHITESPACE) {
        $name[1] = $token[1];
      }
    }

    return $ret;
  }

  private $class_;
  private $tokens_;
}