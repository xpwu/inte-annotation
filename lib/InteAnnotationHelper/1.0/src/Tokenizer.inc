<?php


namespace Inte\Annotation\Helper;


use ReflectionClass;
use ReflectionProperty;

class Tokenizer {
  function __construct(ReflectionClass $class) {
    $this->class_ = $class;
    $this->tokens_ = token_get_all(
      file_get_contents($this->class_->getFileName()));
  }

  public function directlyImplementInterface(ReflectionClass $interface):bool {
    $alias = $this->getAliasNames($interface);
    $implements = $this->getAllImplements();

    return count(array_intersect($alias, $implements));
  }

  public function getAliasNames(ReflectionClass $interfaceOrClass):array {
    $alias = [];

    // 1  interface 的命名空间是class的子命名空间, 或者同命名空间
    if ($this->class_->getNamespaceName() !== null
      && $this->class_->getNamespaceName() !== "") {

      $pos = strpos($interfaceOrClass->getName()
        , $this->class_->getNamespaceName());
      if ($pos !== false) {
        $alias[] = substr($interfaceOrClass->getName()
          , strlen($this->class_->getNamespaceName()) + $pos+1/* 去掉\ */);
      }
    }

    // 2 使用 use ... [as] ...  引入的

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
        if ($name[0] === $interfaceOrClass->getName()) {
          $alias[] = $name[1];
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

  private function getAllImplements():array {
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
   * return: string|bool|int|float|其他类型|前面类型的数组  一个变量可以有多个类型 | 分隔
   *  '@var' 后面定义的类型，就是返回值，没有此注释就返回null，| 可以分隔多个类型
   *
   * @throws \ReflectionException
   */
  public function getPropertyTypeName(string $propertyName):array {

    $refProperty = $this->class_->getProperty($propertyName);
    $doc = $refProperty->getDocComment();
    $matched = preg_match(
      '/^\s*\*\s*@var\s+?'
      .'((\\\\??[a-zA-Z_\x7f-\xff][a-zA-Z0-9_\x7f-\xff]*?)+?)'
      .'\s+?/mU'
      , $doc, $matches);

    $ret = [];

    if ($matched === false || $matched === 0
      || count($matches) !== 3 || $matches[1] === null) {
      return $ret;
    }

    $primitive = [
      'string'=>1,
      'bool'=>1,
      'int'=>1,
      'float'=>1,
      'array'=>1,
      'object'=>1,
      'null'=>1,
      'callback'=>1,
      'mixed'=>1,
      'resource'=>1,
      ];

    $types = explode('|', $matches[1]);

    foreach ($types as $type) {
      if (@$primitive[$type] !== null) {
        $ret[] = $type;
        continue;
      }

      if (strpos($type, '\\') === 0) {
        $ret[] = substr($type, 1);
        continue;
      }

      $uses = $this->getAllUseMap();
      $fullNm = @$uses[$type];
      if ($fullNm !== null) {
        $ret[] = $fullNm;
        continue;
      }

      $ret[] = $this->class_->getNamespaceName() . '\\' . $type;
    }

    return $ret;
  }

  /**
   * @return [as=>fullName] array
   */
  private function getAllUseMap():array {
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