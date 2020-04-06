As a broad design goal I wanted to make an AI that evaluated the playing field and figured out a strategy to use at the moment.
This led me to make a utility-based AI that scores the mobs on the field using their damage, health and other attributes then
chooses from a list of strategies based on whether they have a plan for it. The main idea behind my design was to make it as easy
as possible to add more strategies and change the overall "persona" of the AI by switching out what strategies it uses. What I
did was make an interface (iStrategy) that defined shouldExecute(gameStateVariables) and execute(Player, gameStateVariables).
These functions served as the core of every strategy, where shouldExecute determined when a strategy recognizes the game state as
something it can work with and execute performs whatever action the strategy wants to do. The AI has a list of these strategies
and iterates through them calling shouldExecute to look for a strategy to perform. It then calls execute one whichever strategy
returned true on shouldExecute first (or on the last strategy in the list if all return false). There is also a notion of 
"locking in" a strategy where execute returns true and the index of the strategy is saved by the AI to immediate call execute on
next frame. This allows for more long winded strategies that would be hard to execute if consitantly evaluating shouldExecute. 
In terms of stretch goals, I did make the evaluation function only care about the unit stats so it should work with any new units.

For my specific implementation of this AI, I made three strategies: Attack, Defend, and Counter. The most simple of these is 
Defend, which simply evaluates whether the enemy units pose a threat and then tries to kill them. Attack is most interesting, as
it has three behaviors. Attack will launch a big attack with a Giant and two archers if it has enough elixir. It will also try to
keep an attack going by spawning in supporting archers, or finish off a low health tower by spawning in a swordman an archer.
Counter is definitely the most interesting as it will try to attack a side if the other is very strong (has more units) and it
performs a "Bait and Switch." Counter spawns in a swordsman on one of the sides and waits for the other player to commit elixir
towards either defending against it or attacking the other side and then spawns an attacking force on the weaker side. Though I
don't really consider myself as an AI designer, I feel like this AI has some pretty interesting strategies and feels like it
could be another player (other than the multi unit placement on one frame).

