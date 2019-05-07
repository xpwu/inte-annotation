<?php


namespace Inte\Annotation;



use Inte\Annotation\Helper\ClassHelperStrategy;
use Inte\Annotation\Helper\HelperStrategy;
use Inte\Annotation\Helper\InterfaceHelperStrategy;
use Inte\MetaAnnotation\Annotation;

class Helper {
  public function __construct(\ReflectionClass $annotation, Logger $logger) {
    if ($annotation->isInterface()) {
      $this->strategy_ = new InterfaceHelperStrategy($annotation);
      return;
    }

    $this->strategy_ = new ClassHelperStrategy($annotation, $logger);
  }

  public function isAnnotatingClass(\ReflectionClass $class):bool {
    return $this->strategy_->isAnnotatingClass($class);
  }

  public function getAnnotationAssociatedWithMethod(
    \ReflectionMethod $method):?Annotation {

    return $this->strategy_->getAnnotationUsedByMethod($method);
  }

  public function getAnnotationAssociatedWithProperty(
    \ReflectionProperty $property):?Annotation {

    return $this->strategy_->getAnnotationUsedByProperty($property);
  }

  /**
   * @var HelperStrategy
   */
  private $strategy_;
}