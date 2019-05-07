<?php


namespace Inte\Annotation\Helper;

use Inte\Annotation\Logger;
use Inte\MetaAnnotation\Annotation;
use Inte\MetaAnnotation\Inherited;


class ClassHelperStrategy implements HelperStrategy {

  function __construct(\ReflectionClass $annotation, Logger $logger) {
    $this->logger_ = $logger;
    $this->annotation_ = $annotation;
  }

  public function isAnnotatingClass(\ReflectionClass $class):bool {
    if (!$class->isSubclassOf($this->annotation_->getName())) {
      return false;
    }

    if ($this->annotation_->implementsInterface(Inherited::class)) {
      return true;
    }

    if ($class->getParentClass()->getName() === $this->annotation_->getName()) {
      return true;
    }

    return false;
  }


  public function getAnnotationUsedByMethod(
    \ReflectionMethod $method):?Annotation {

    return $this->getAnnotationUsedByMethodOrProperty($method);
  }

  /**
   * @param \ReflectionMethod|\ReflectionProperty $methodOrProperty
   * @return Annotation|null
   */
  private function getAnnotationUsedByMethodOrProperty(
    $methodOrProperty):?Annotation {

    if ($this->annotation_->isAbstract()) {
      $this->logger_->throwError(
        "annotation, used by method or property, need be is Not-abstract");
    }

    $class = $methodOrProperty->getDeclaringClass();
    $tokenizer = new Tokenizer($class);
    $allAlias = $tokenizer->getAliasNames($this->annotation_);

    $doc = $methodOrProperty->getDocComment();

    $matches = [];
    $matched = 0;
    foreach ($allAlias as $alias) {
      $alias = str_replace('\\', '\\\\', $alias);

      //  * uses annotation (a=c,x="y")
      $matched = preg_match(
        '/^\s*\*\s*@uses\s+'.$alias
        .'(\s+\((.*)\))?\s*$/mU', $doc, $matches);

      if ($matched !== 0) {
        break;
      }
    }

    if ($matched === 0) {
      return null;
    }

    $annotation = $this->annotation_->newInstance();
    if (count($matches) === 3 && $matches[2] !== null) {
      $args = explode(',', $matches[2]);
      foreach ($args as $arg) {
        list($key, $value) = explode('=', $arg);
        if ($value === null) {
          $value = $key;
          $key = "value";
        }
        $annotation->{$key} = $value;
      }
    }

    /** @var Annotation $annotation */
    $this->checkAnnotationUsedByMethodOrProperty($annotation, $methodOrProperty);

    return $annotation;
  }

  /**
   * @param Annotation $annotation
   * @param \ReflectionProperty|\ReflectionMethod $annotatedItem
   */
  private function checkAnnotationUsedByMethodOrProperty(
    Annotation $annotation, $annotatedItem):void {

    foreach ($annotation as $key=>$value) {
      if ($value === null) {
        $this->logger_->throwError(
          "annotation(@".$this->annotation_->getName()."), at "
          . $annotatedItem->getDeclaringClass()->getName()
          . "::" . $annotatedItem->getName()
          . "(file: ". $annotatedItem->getDeclaringClass()->getFileName()
          ." ; line:". $annotatedItem->getStartLine()
          ."), arg($key) NOT set");
      }
    }
  }

  public function getAnnotationUsedByProperty(
    \ReflectionProperty $property):?Annotation {

    return $this->getAnnotationUsedByMethodOrProperty($property);
  }

  private $annotation_;
  /**
   * @var Logger
   */
  private $logger_;
}