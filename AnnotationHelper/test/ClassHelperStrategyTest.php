<?php

require_once "../src/HelperStrategy.inc";
require_once "../src/ClassHelperStrategy.inc";
require_once "TestGetAliasNames.inc";
require_once "MockLogger.inc";
require_once "../src/Tokenizer.inc";
require_once "AnnotationTestClass.inc";

use Inte\Annotation\Helper\ClassHelperStrategy;
use PHPUnit\Framework\TestCase;

class ClassHelperStrategyTest extends TestCase {

  public function testGetAnnotationUsedByProperty() {
    $annotation = new ReflectionClass(AnnotationTestClass::class);

    $testClass = new ReflectionClass(\Inte\Test\TestGetAliasNames::class);

    $helper = new ClassHelperStrategy(
      $annotation, new MockLogger());

    /**
     * @var AnnotationTestClass $ret
     */
    $ret = $helper->getAnnotationUsedByProperty($testClass->getProperty("b"));

    self::assertNotEquals(null, $ret);
    self::assertEquals('ok', $ret->value);
    self::assertEquals(true, $ret->arg[3]);
  }
}
