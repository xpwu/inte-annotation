<?php


require_once "../../lib/InteCoreAnnotation/1.0/Annotation/Logger.inc";

class MockLogger implements \Inte\Annotation\Logger {

  /**
   * @param string $msg
   * @throws ErrorException
   */
  public function throwError(string $msg): void {
    throw new ErrorException($msg);
  }

  public function warn(string $msg): void {
    echo $msg . PHP_EOL;
  }

  public function info(string $msg): void {
    echo $msg . PHP_EOL;
  }
}