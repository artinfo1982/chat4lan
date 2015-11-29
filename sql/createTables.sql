SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0;
SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0;
SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='TRADITIONAL,ALLOW_INVALID_DATES';

CREATE SCHEMA IF NOT EXISTS `chatDB` DEFAULT CHARACTER SET latin1 ;
USE `chatDB` ;

-- -----------------------------------------------------
-- Table `chatDB`.`usr_info`
-- -----------------------------------------------------
CREATE  TABLE IF NOT EXISTS `chatDB`.`usr_info` (
  `usr_id` INT UNSIGNED NOT NULL AUTO_INCREMENT ,
  `usr_name` VARCHAR(64) BINARY NOT NULL ,
  `usr_password` VARCHAR(64) BINARY NOT NULL ,
  `usr_friend_num` INT UNSIGNED NOT NULL ,
  `usr_group_num` INT UNSIGNED NOT NULL ,
  `usr_status` INT NOT NULL ,
  `usr_local_ip` VARCHAR(16) NOT NULL ,
  `usr_local_port` INT UNSIGNED NOT NULL ,
  PRIMARY KEY (`usr_id`) ,
  UNIQUE INDEX `usr_name_UNIQUE` (`usr_name` ASC) ,
  UNIQUE INDEX `usr_id_UNIQUE` (`usr_id` ASC) )
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `chatDB`.`usr_friend_info`
-- -----------------------------------------------------
CREATE  TABLE IF NOT EXISTS `chatDB`.`usr_friend_info` (
  `index` INT NOT NULL AUTO_INCREMENT ,
  `usr_id` INT UNSIGNED NOT NULL ,
  `usr_friend_id` INT UNSIGNED NOT NULL ,
  PRIMARY KEY (`index`) ,
  UNIQUE INDEX `index_UNIQUE` (`index` ASC) )
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `chatDB`.`usr_group_info`
-- -----------------------------------------------------
CREATE  TABLE IF NOT EXISTS `chatDB`.`usr_group_info` (
  `index` INT NOT NULL AUTO_INCREMENT ,
  `usr_id` INT UNSIGNED NOT NULL ,
  `group_id` INT UNSIGNED NOT NULL ,
  PRIMARY KEY (`index`) ,
  UNIQUE INDEX `index_UNIQUE` (`index` ASC) )
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `chatDB`.`group_info`
-- -----------------------------------------------------
CREATE  TABLE IF NOT EXISTS `chatDB`.`group_info` (
  `group_id` INT UNSIGNED NOT NULL AUTO_INCREMENT ,
  `group_name` VARCHAR(64) BINARY NOT NULL ,
  `group_member_num` INT UNSIGNED NOT NULL ,
  `group_online_member_num` INT UNSIGNED NOT NULL ,
  PRIMARY KEY (`group_id`) ,
  UNIQUE INDEX `group_id_UNIQUE` (`group_id` ASC) ,
  UNIQUE INDEX `group_name_UNIQUE` (`group_name` ASC) )
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `chatDB`.`group_member_info`
-- -----------------------------------------------------
CREATE  TABLE IF NOT EXISTS `chatDB`.`group_member_info` (
  `index` INT NOT NULL AUTO_INCREMENT ,
  `group_id` INT UNSIGNED NOT NULL ,
  `member_id` INT UNSIGNED NOT NULL ,
  PRIMARY KEY (`index`) ,
  UNIQUE INDEX `index_UNIQUE` (`index` ASC) )
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `chatDB`.`usr_resend_info`
-- -----------------------------------------------------
CREATE  TABLE IF NOT EXISTS `chatDB`.`usr_resend_info` (
  `index` INT NOT NULL AUTO_INCREMENT ,
  `usr_id` INT UNSIGNED NOT NULL ,
  `usr_friend_id` INT UNSIGNED NOT NULL ,
  `usr_resend_msg` VARCHAR(1024) BINARY NOT NULL ,
  PRIMARY KEY (`index`) ,
  UNIQUE INDEX `index_UNIQUE` (`index` ASC) )
ENGINE = InnoDB;



SET SQL_MODE=@OLD_SQL_MODE;
SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS;
SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS;
