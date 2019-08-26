<?php declare(strict_types=1);


namespace Inte\Annotation\Helper;


class Comment {
  private $str_;

  function __construct(string $commentStr) {
    $this->str_ = $commentStr;
  }

  /**
   * @param string ...$excludeTags
   * @return string[]
   */
  public function exclude(string ...$excludeTags):array {
    $lines = explode("\n", $this->str_);
    $ret = [];
    foreach ($lines as $line) {
      $matched = false;
      foreach ($excludeTags as $excludeTag) {
        // * tag
        $match = preg_match(
          '/^\s*\*\s*'.$excludeTag.'\s*/mU', $line);

        if ($match !== 0) {
          $matched = true;
          break;
        }
      }
      if ($matched) {
        continue;
      }
      // /**
      if (preg_match(
        '/^\s*\/\*\*\s*/mU', $line)) {
        continue;
      }
      // *
      if (preg_match(
        '/^\s*\*\s*$/mU', $line)) {
        continue;
      }
      // */
      if (preg_match(
        '/^\s*\*\/\s*/mU', $line)) {
        continue;
      }

      $temp = trim($line);
      if (strlen($temp) > 1 && $temp[0] === '*') {
        $temp = trim(substr($temp, 1));
      }

      $ret[] = $temp;
    }

    return $ret;
  }

  /**
   * @param string $tag
   * @return string
   *
   * @deprecated
   */
  public function getValue(string $tag):string {
    $lines = explode("\n", $this->str_);
    foreach ($lines as $line) {
      // * tag
      $matched = preg_match(
        '/^\s*\*\s*'.$tag.'(.*)$/mU', $line, $matches);

      if ($matched !== 0) {
        break;
      }
    }

    if (count($matches) < 2 || $matches[1] == null) {
      return "";
    }

    return trim($matches[1]);
  }

  /**
   * @param string $tag
   * @return string[]
   */
  public function getTagValues(string $tag):array {
    $ret = [];

    $lines = explode("\n", $this->str_);

    foreach ($lines as $line) {
      // * tag
      $matched = preg_match(
        '/^\s*\*\s*'.$tag.'(.*)$/mU', $line, $matches);

      if ($matched === 0) {
        continue;
      }

      if (count($matches) < 2 || $matches[1] == null) {
        $ret[] = "";
      }

      $ret[] = trim($matches[1]);
    }

    return $ret;
  }

}