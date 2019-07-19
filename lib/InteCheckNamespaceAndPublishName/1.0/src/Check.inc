<?php


namespace Inte\CheckNamespaceAndPublishName;

use Inte\Annotation\AbstractAnnotationServer;
use Inte\Annotation\Config;
use Inte\Annotation\Logger;
use Inte\Annotation\PhpFiler;

class Check extends AbstractAnnotationServer {

  /**
   * @param string[] $classes
   */
  function process(array $classes): void {
    if ($this->isBuild_) {
      return;
    }

    $publishName = $this->config_->getPublishConfig()->getName();
    foreach ($classes as $class) {
      $classN = str_replace('\\', '', $class);
      $pos = strpos($classN, $publishName);
      // 修改为不要求必须是发布名开头，可能部分库的类名有固定前缀
      if ($pos === false) {
        $this->logger_->throwError(
          "$class name is Not matched with publish name($publishName)");
      }
    }
  }

  function init(Logger $logger, PhpFiler $filer
    , Config $config, string $projectRootDir, bool $isBuild): void {

    $this->logger_ = $logger;
    $this->config_ = $config;
    $this->isBuild_ = $isBuild;
  }

  /**
   * @var Logger
   */
  private $logger_;
  /**
   * @var Config
   */
  private $config_;
  private $isBuild_;
}