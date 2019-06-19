<?php


namespace Inte\Annotation\Helper;


use Inte\MetaAnnotation\Inherited;
use Inte\MetaAnnotation\Annotation;

class InterfaceHelperStrategy implements HelperStrategy {
  public function __construct(\ReflectionClass $annotation) {
    $this->annotation_ = $annotation;
    $tokenizer = new Tokenizer($this->annotation_);
    $this->inherited_ = $tokenizer->directlyImplementInterface(
      new \ReflectionClass(Inherited::class));
  }

  public function isAnnotatingClass(\ReflectionClass $class):bool {
    if (!$class->implementsInterface($this->annotation_->getName())) {
      return false;
    }

//    if ($this->annotation_->isSubclassOf(Inherited::class)) {
//      return true;
//    }
    // 注解的继承属性必须是明确在注解的定义中是了Inherited元注解，扩展的其他被Inherited注解的
    //  注解不认为 此注解不具有继承属性。也就是说Inherited元注解本身不具有继承属性。
    if ($this->inherited_) {
      return true;
    }


    $tokenizer = new Tokenizer($class);
    return $tokenizer->directlyImplementInterface($this->annotation_);
  }

  public function isAnnotatingInterface(\ReflectionClass $class):bool {
    if (!$class->isSubclassOf($this->annotation_->getName())) {
      return false;
    }

    if ($this->annotation_->isSubclassOf(Inherited::class)) {
      return true;
    }

    $tokenizer = new InterfaceTokenizer($class);
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
  private $inherited_ = false;

}