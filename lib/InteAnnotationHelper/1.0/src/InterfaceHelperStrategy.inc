<?php


namespace Inte\Annotation\Helper;


use Inte\MetaAnnotation\Inherited;
use Inte\MetaAnnotation\Annotation;

class InterfaceHelperStrategy implements HelperStrategy {
  public function __construct(\ReflectionClass $annotation) {
    $this->annotation_ = $annotation;
  }

  public function isAnnotatingClass(\ReflectionClass $class):bool {
    if (!$class->implementsInterface($this->annotation_->getName())) {
      return false;
    }

    if ($this->annotation_->implementsInterface(Inherited::class)) {
      return true;
    }

    $tokenizer = new Tokenizer($class);
    return $tokenizer->directlyImplementInterface($this->annotation_);
  }


  public function getAnnotationUsedByMethod(
    \ReflectionMethod $method):?Annotation {

    /**
     * 接口类型的注解 用在方法与属性上无效
     */
    return null;
  }


  public function getAnnotationUsedByProperty(
    \ReflectionProperty $property):?Annotation {

    /**
     * 接口类型的注解 用在方法与属性上无效
     */
    return null;
  }

  private $annotation_;

}