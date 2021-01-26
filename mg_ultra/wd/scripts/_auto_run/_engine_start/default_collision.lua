-- \scripts\_auto_run\_engine_start\default_collision.lua

-- Handler for player bullet hitting enemy
Collision.add_event(EntityEnemy, EntityPlayerBullets)

-- Handler for player bullet hitting boss
Collision.add_event(EntityBoss, EntityPlayerBullets, "Boss.damage_handle()")

-- Handler for player being hit by enemy bullet
Collision.add_event(EntityPlayer, EntityEnemyBullet, "g_bulletPlayerCollision()")

-- Handler for bullet clearer hitting enemy bullets
Collision.add_event(EntityBulletClearer, EntityEnemyBullet)