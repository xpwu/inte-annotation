<?php


namespace Inte\Annotation\Helper;

require_once "../src/Tokenizer.inc";
require_once "TestGetAliasNames.inc";

use Inte\Test\ElseClass\TestGetAliasNamesElseClass;
use Inte\Test\TestGetAliasNames;
use PHPUnit\Framework\TestCase;

class TokenizerTest extends TestCase {

  public function testGetAliasNames() {
    $ref = new \ReflectionClass(TestGetAliasNames::class);
    $tokenizer = new Tokenizer($ref);
    $ret = $tokenizer->getAliasNames(
      new \ReflectionClass(TestGetAliasNamesElseClass::class));
    self::assertIsArray($ret, implode(",", $ret));
    self::assertCount(4, $ret, implode(",", $ret));
  }

  public function testDirectlyImplementInterface() {
    $ref = new \ReflectionClass(TestGetAliasNames::class);
    $tokenizer = new Tokenizer($ref);
    $ret = $tokenizer->directlyImplementInterface(
      new \ReflectionClass(TestGetAliasNamesElseClass::class));
    self::assertEquals(true, $ret);
  }
}
