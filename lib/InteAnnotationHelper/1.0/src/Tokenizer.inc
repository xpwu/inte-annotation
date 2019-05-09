<?php


namespace Inte\Annotation\Helper;


use ReflectionClass;

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

      if (is_array($token) && $token[0] === T_AS) {
        $process = 1;
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

  private $class_;
  private $tokens_;
}