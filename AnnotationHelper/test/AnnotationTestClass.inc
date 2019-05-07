<?php


require_once "../../lib/InteMetaAnnotation/1.0/src/Annotation.inc";

class AnnotationTestClass implements \Inte\MetaAnnotation\Annotation {
  public $value;
  public $arg;
}