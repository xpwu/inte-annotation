<?php


namespace Inte\Annotation\Helper;



use Inte\MetaAnnotation\Annotation;

interface HelperStrategy {
  public function isAnnotatingClass(\ReflectionClass $class):bool ;

  public function isAnnotatingInterface(\ReflectionClass $class):bool ;

  public function getAnnotationUsedByMethod(
    \ReflectionMethod $method):?Annotation ;


  public function getAnnotationUsedByProperty(
    \ReflectionProperty $property):?Annotation ;

}