/*
Navicat MySQL Data Transfer

Source Server         : localhost
Source Server Version : 50548
Source Host           : localhost:3306
Source Database       : product_db

Target Server Type    : MYSQL
Target Server Version : 50548
File Encoding         : 65001

Date: 2018-09-07 17:15:17
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for t_barcode
-- ----------------------------
DROP TABLE IF EXISTS `t_barcode`;
CREATE TABLE `t_barcode` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `father` varchar(255) NOT NULL,
  `barcode` varchar(255) DEFAULT NULL,
  `flag` int(11) NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 COMMENT='father  唯一码 用与和垛码表进行关联\r\nbarcode 条码\r\nflag   是否已生成垛码的标志 0 为未生成，1为已生成';

-- ----------------------------
-- Records of t_barcode
-- ----------------------------

-- ----------------------------
-- Table structure for t_barcode_rule
-- ----------------------------
DROP TABLE IF EXISTS `t_barcode_rule`;
CREATE TABLE `t_barcode_rule` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `manufacturer_code` varchar(255) CHARACTER SET gbk NOT NULL,
  `product_code_len` int(11) NOT NULL DEFAULT '5',
  `workshop_no_postion` int(11) NOT NULL DEFAULT '0',
  `manufacturer_name` varchar(255) CHARACTER SET gbk DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=6 DEFAULT CHARSET=latin1 COMMENT='产品条码规则表\r\nmanufacturer_code 厂商代码\r\nproduct_code_len 产品码长度\r\nworkshop_no_postion  车间号的倒数位置长度为2，为0时表示条码不包含车间号默认赋0\r\nmanufacturer_name 厂商名称';

-- ----------------------------
-- Records of t_barcode_rule
-- ----------------------------
INSERT INTO `t_barcode_rule` VALUES ('1', '6904881', '5', '6', '燃气具公司');
INSERT INTO `t_barcode_rule` VALUES ('2', '697052662', '3', '6', '厨房科技');
INSERT INTO `t_barcode_rule` VALUES ('3', '69507245', '4', '6', '华润代工产品');
INSERT INTO `t_barcode_rule` VALUES ('4', '11104', '3', '0', '中燃宝代工产品');

-- ----------------------------
-- Table structure for t_buttress_code
-- ----------------------------
DROP TABLE IF EXISTS `t_buttress_code`;
CREATE TABLE `t_buttress_code` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `unique_code` varchar(255) NOT NULL,
  `buttress_code` varchar(255) NOT NULL,
  `upload` int(11) NOT NULL DEFAULT '0',
  `date` datetime NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=gbk COMMENT='unique_code 唯一码与t_barcode 中father 关联\r\nbuttress_code 垛码\r\nupload  是否上传到服务器 0 为未上传 1为已上传\r\ndate  生成垛码的时间方便以后自动清表';

-- ----------------------------
-- Records of t_buttress_code
-- ----------------------------

-- ----------------------------
-- Table structure for t_product_info
-- ----------------------------
DROP TABLE IF EXISTS `t_product_info`;
CREATE TABLE `t_product_info` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `code` varchar(255) NOT NULL,
  `erp_code` varchar(255) NOT NULL,
  `product_desc` varchar(1024) DEFAULT NULL,
  `kind` varchar(255) NOT NULL,
  `name` varchar(1024) DEFAULT NULL,
  `maintain_date` datetime DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=gbk COMMENT='code 产品条码\r\nkind 产品类型';

-- ----------------------------
-- Records of t_product_info
-- ----------------------------
