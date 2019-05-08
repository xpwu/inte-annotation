<?php

require_once "../../lib/InteMetaAnnotation/1.0/src/Inherited.inc";
require_once "../src/HelperStrategy.inc";
require_once "../src/InterfaceHelperStrategy.inc";
require_once "../src/Tokenizer.inc";
require_once "../src/InterfaceTokenizer.inc";
require_once "InterfaceTest.inc";

use Inte\Annotation\Helper\InterfaceHelperStrategy;
use Inte\Test\InterfaceTest;
use PHPUnit\Framework\TestCase;

class HelperStrategyTest extends TestCase {

  public function testIsAnnotatingInterface() {
    $helper = new InterfaceHelperStrategy(
      new ReflectionClass(
        \Inte\Test\ElseClass\TestGetAliasNamesElseClass::class));

    $ref = new \ReflectionClass(InterfaceTest::class);

    $ret = $helper->isAnnotatingInterface($ref);

    self::assertEquals(true, $ret);
  }

}
