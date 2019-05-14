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

  /**
   * @throws \ReflectionException
   */
  public function testGetPropertyTypeName() {
    $tokenize = new Tokenizer(new \ReflectionClass(TestGetAliasNames::class));
    $types = $tokenize->getPropertyTypeName("a");
    self::assertEquals(1, count($types));
    self::assertEquals(TestGetAliasNamesElseClass::class, $types[0]);

    $types = $tokenize->getPropertyTypeName("b");
    self::assertEquals(1, count($types));
    self::assertEquals(TestGetAliasNamesElseClass::class, $types[0]);

    $types = $tokenize->getPropertyTypeName("c");
    self::assertEquals(1, count($types));
    self::assertEquals(TestGetAliasNamesElseClass::class, $types[0]);

    $types = $tokenize->getPropertyTypeName("d");
    self::assertEquals(1, count($types));
    self::assertEquals(TestGetAliasNamesElseClass::class, $types[0]);

    $types = $tokenize->getPropertyTypeName("e");
    self::assertEquals(0, count($types));

    $types = $tokenize->getPropertyTypeName("f");
    self::assertEquals(1, count($types));
    self::assertEquals('int', $types[0]);

    $types = $tokenize->getPropertyTypeName("g");
    self::assertEquals(2, count($types));
    self::assertEquals('int', $types[0]);
    self::assertEquals('null', $types[1]);

    $types = $tokenize->getPropertyTypeName("h");
    self::assertEquals(2, count($types));
    self::assertEquals('int[]', $types[0]);
    self::assertEquals('float', $types[1]);

    $types = $tokenize->getPropertyTypeName("i");
    self::assertEquals(1, count($types));
    self::assertEquals('Inte\Test\ElseClass\TestGetAliasNamesElseClass[]'
      , $types[0]);

    $types = $tokenize->getPropertyTypeName("j");
    self::assertEquals(2, count($types));
    self::assertEquals('Inte\Test\ElseClass\TestGetAliasNamesElseClass[]'
      , $types[0]);
    self::assertEquals('bool', $types[1]);

    $types = $tokenize->getPropertyTypeName("k");
    self::assertEquals(2, count($types));
    self::assertEquals('Inte\Test\ElseClass\TestGetAliasNamesElseClass[]'
      , $types[0]);
    self::assertEquals('bool[]', $types[1]);

    $types = $tokenize->getPropertyTypeName("l");
    self::assertEquals(0, count($types));

  }
}
