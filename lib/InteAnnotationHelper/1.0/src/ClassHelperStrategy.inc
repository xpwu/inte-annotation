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

  public function isAnnotatingInterface(\ReflectionClass $class):bool {
    // 类的注解器通过继承，通过继承方式，不可能注解到一个接口上
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
    if (count($matches) === 3 && $matches[2] !== null
      && trim($matches[2]) !== "") {

      $subject = trim($matches[2]);

      // 替换'[]'中的','为';'  方便下面做切分。同时去掉'[]'中的最后一个',' 。
//      $argStrs = preg_replace('/\[[^,]+,[^,]*\]/')
//      $matched = preg_match_all('/\[([^,]+)?((,)[^,]+)+(,)??(\])/', $subject
//        , $indexMatches, PREG_SET_ORDER|PREG_OFFSET_CAPTURE);

      $argPatten =
        '/(\s*([a-zA-Z_][a-zA-Z0-9_]*\s*=)+\s*?((\'.*\')|(".*")|(\[.*\])|(.+))\s*?,?)/';
      $matched = preg_match_all($argPatten, $subject, $args, PREG_SET_ORDER);
      if ($matched === false || $matched === 0) {
        $this->logger_->throwError($this->log($methodOrProperty)
          . ". \nArg is error, which must be $argPatten");
      }


      $args = explode(',', $matches[2]);
      foreach ($args as $arg) {
        // 对于不写key的参数值，赋值给value变量
        $arr = explode('=', $arg);
        $arr[] = null; // 防止$arr的长度为1 引起list crash
        list($key, $value) = $arr;
        if ($value === null) {
          $value = $key;
          $key = "value";
        }
        $value = trim($value);
        $key = trim($key);
        $annotation->{$key} = $this->unSerialize($value);
      }
    }

    /** @var Annotation $annotation */
    $this->checkAnnotationUsedByMethodOrProperty($annotation, $methodOrProperty);

    return $annotation;
  }

  private function unSerialize(string $str) {
    if ($str === 'false' || $str === 'FALSE') return false;
    if ($str === 'true' || $str === 'TRUE') return true;
    if ($str === "") return null;
    if ($str === "null") return null;
    if (strlen($str) >= 2 && (($str[0] === '"' && $str[-1] === '"')
      || ($str[0] === '\'' && $str[-1] === '\'')))
      return substr($str, 1, -1);

    /**
     * to number
     * @var int|float|double $str
     */
    if (is_numeric($str)) return 0 + $str;

    if (strlen($str) >= 2 && $str[0] === '[' && $str[-1] === ']') {
      $ret = [];
      $arr = explode(';', substr($str, 1, -1));
      foreach ($arr as $item) {
        $r = $this->unSerialize(trim($item));
        if ($r === null) {
          return null;
        }
        $ret[] = $r;
      }
      return $ret;
    }

    return $str;
  }

  /**
   *
   * @param \ReflectionProperty|\ReflectionMethod $annotatedItem
   */
  private function log($annotatedItem):string {
    return "annotation(@".$this->annotation_->getName()."), at "
    . $annotatedItem->getDeclaringClass()->getName()
    . "::" . $annotatedItem->getName()
    . "(file: ...". substr(
      $annotatedItem->getDeclaringClass()->getFileName(), -25)
    .")";
  }

  /**
   * @param Annotation $annotation
   * @param \ReflectionProperty|\ReflectionMethod $annotatedItem
   */
  private function checkAnnotationUsedByMethodOrProperty(
    Annotation $annotation, $annotatedItem):void {
    $properties = $this->annotation_->getProperties(
      \ReflectionProperty::IS_PUBLIC);
    $proNames = [];
    foreach ($properties as $property) {
      $proNames[$property->getName()] = 1;
    }

    foreach ($annotation as $key=>$value) {
      if (!array_key_exists($key, $proNames)) {
        $this->logger_->throwError($this->log($annotatedItem)
          . ". NOT exist args(public property of annotation) '$key'");
      }

      if ($value === null) {
        $this->logger_->throwError($this->log($annotatedItem)
          . ". NOT set or value type is INVALID");
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