CREATE TABLE `users` (
    `id` INT UNSIGNED NOT NULL AUTO_INCREMENT,
    `name` VARCHAR(24) NOT NULL,
    `password` VARCHAR(20) NOT NULL,
    `isLoggedIn` TINYINT(1) NOT NULL DEFAULT 0,
    PRIMARY KEY (`id`),
    UNIQUE KEY `uniq_name` (`name`),
    KEY `idx_isLoggedIn` (`isLoggedIn`)
);