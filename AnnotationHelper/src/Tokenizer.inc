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

  public function getAliasNames(ReflectionClass $interface):array {
    $alias = [];
    // 1  interface 的命名空间是class的子命名空间, 或者同命名空间
    $pos = strpos($interface->getName(), $this->class_->getNamespaceName());
    if ($pos !== false) {
      $alias[] = substr($interface->getName(), $pos+1/* 去掉\ */);
    }

    // 2 使用 use ... [as] ...  引入的

    $process = -1;
    $name = ["", $interface->getShortName()]; // use=>as
    foreach ($this->tokens_ as $token) {
      if (is_array($token) && $token[0] === T_USE) {
        $process = 0;
        continue;
      }

      if (is_array($token) && $token[0] === T_AS) {
        $process = 1;
      }

      if ($token === ";" && $process != -1) {
        $process = -1;
        if ($name[0] === $interface->getName()) {
          $alias[] = $name[1];
          break;
        }
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
    $alias[] = "\\" . $interface->getName();

    return $alias;
  }

  private function getAllImplements():array {
    $implements = [];

    $thisClass = 0;
    foreach ($this->tokens_ as $token) {
      if (is_array($token) && $token[0] === T_CLASS
        && $token[1] === $this->class_->getShortName()) {
        $thisClass = 1;
        continue;
      }

      if ($thisClass != 0 && $token === "{") {
        break;
      }

      if ($thisClass == 1 && is_array($token) && $token[0] === T_IMPLEMENTS) {
        $thisClass = 2;
        continue;
      }

      if ($thisClass == 2 && is_array($token) && $token[0] === T_STRING) {
        $implements[] = $token[1];
      }
    }

    return $implements;
  }

  private $class_;
  private $tokens_;
}