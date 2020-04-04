-- \scripts\_auto_run\_engine_start\seed_random.lua

-- Initialise random
math.randomseed(os.time())

-- Pop some random numbers
for i = 1,10 do math.random() end