-- \scripts\_auto_run\_engine_start\default_collision.lua

--Handler for player bullet hitting enemy
Collision.add_event(EntityPlayerBullets, EntityEnemy)

--Handler for player being hit by enemy bullet
Collision.add_event(EntityPlayer, EntityEnemyBullet, "g_bulletPlayerCollision()")