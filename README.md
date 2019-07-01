# kingpin_hook
Simple (or not?) clienthook for Kingpin game

I've included a "memory manager" (sort of) recently and it just werks.
It calls game's memory cleaning funcs on dead characters, so that you could spawn more of them (up to the 64 chars max (including player)).
Usually, when they die, they're still being present in the game's memory and once you hit the limit of 64 chars, you can't spawn any more, my memory manager kinda fixes that, although it's ghetto: if someone's died, his body will disappear due to the cleaning. If you don't want for this to happen, simply comment the call out (it's somewhere in the hooks, can't remember now).
