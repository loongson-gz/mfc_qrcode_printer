/*
Navicat MySQL Data Transfer

Source Server         : localhost
Source Server Version : 50548
Source Host           : localhost:3306
Source Database       : product_db

Target Server Type    : MYSQL
Target Server Version : 50548
File Encoding         : 65001

Date: 2018-09-06 23:13:13
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for t_barcode
-- ----------------------------
DROP TABLE IF EXISTS `t_barcode`;
CREATE TABLE `t_barcode` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `barcode` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 COMMENT='barcode 条码';

-- ----------------------------
-- Table structure for t_barcode_rule
-- ----------------------------
DROP TABLE IF EXISTS `t_barcode_rule`;
CREATE TABLE `t_barcode_rule` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `manufacturer_code` varchar(255) DEFAULT NULL,
  `manufacturer_code_len` int(11) DEFAULT NULL,
  `product_code_len` int(11) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 COMMENT='产品条码规则表\r\nmanufacturer_code 厂商代码\r\nmanufacturer_code_len 厂商代码长度\r\nproduct_code_len 产品码长度';

-- ----------------------------
-- Table structure for t_product_info
-- ----------------------------
DROP TABLE IF EXISTS `t_product_info`;
CREATE TABLE `t_product_info` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `code` varchar(255) DEFAULT NULL,
  `erp_code` varchar(255) DEFAULT NULL,
  `product_desc` varchar(1024) DEFAULT NULL,
  `kind` varchar(255) DEFAULT NULL,
  `name` varchar(1024) DEFAULT NULL,
  `maintain_date` datetime DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=3965 DEFAULT CHARSET=gbk COMMENT='code 产品条码\r\nkind 产品类型';
