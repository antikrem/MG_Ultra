print("preleved")

BulletSpawner.new(
	"default",
	{
		initialiserScript = false,
		scriptLookUp = {
			[20] = function() print("hello") end,
			[25] = false,
			[500] = function() this:kill() print("hello") end,
			[501] = false
		},
		components = {
			{ComponentPosition, 0, 0, 0},
			{ComponentGraphics, "player"},
			{ComponentOffsetOnce},
			{ComponentDieWithMaster}
		}
	}
)