<?php


namespace Inte\Annotation\Helper;



class FileCreator {

  static public function getNameSpace(
    ?string $shouldCreateFileClass = null):string {

    if ($shouldCreateFileClass === null) {
      $shouldCreateFileClass = static::class;
    }

    try {
      $clazz = new \ReflectionClass($shouldCreateFileClass);
    } catch (\ReflectionException $e) {

    }
    return $clazz->getNamespaceName();
  }

  static public function getSuperShortClassName(
    ?string $shouldCreateFileClass = null):string {

    if ($shouldCreateFileClass === null) {
      $shouldCreateFileClass = static::class;
    }

    try {
      $clazz = new \ReflectionClass($shouldCreateFileClass);
    } catch (\ReflectionException $e) {

    }
    return $clazz->getShortName();
  }

  static public function getCreatedClassName(?string $shouldCreateFileClass = null
    , string  $suffix = "Impl"):string {
    if ($shouldCreateFileClass === null) {
      $shouldCreateFileClass = static::class;
    }

    try {
      $clazz = new \ReflectionClass($shouldCreateFileClass);
    } catch (\ReflectionException $e) {

    }
    return $clazz->getShortName() . $suffix;
  }

  static public function getFileDirAndName(
    ?string $shouldCreateFileClass = null
    , string $fileSuffix = ".inc"):string {

    $name = explode('\\', self::getNameSpace($shouldCreateFileClass));
    $name[] = self::getCreatedClassName($shouldCreateFileClass, "") . $fileSuffix;
    return implode('/', $name);
  }

  static public function getFullClassName(?string $shouldCreateFileClass = null
    , string $suffix = "Impl") {

    if ($shouldCreateFileClass === null) {
      $shouldCreateFileClass = static::class;
    }

    return $shouldCreateFileClass . $suffix;
  }
}