# Quest System
This is a very generic quest system written with Unreal Engine classes in C++. I wanted to create a system that is similarly user friendly as the Gameplay Ability System and I wanted to mimic the way
they handle abilities by having an ability that needs to be unlocked first and stands fully for itself. One major drawback of my system, compared to GAS, it's not replicated, meaning you must do the replication part yourself.
The reason is simple, the project I made this plugin for is a singleplayer only project and ensuring that this plugin would work in multiplayer was not suitable.

# What can it do?
- Creation of Quests with Objectives and Rewards
- Unlocking, Accepting, Starting and Finishing Quests
- Receiving rewards for finishing quests (or parts)
- Quest Progression with generic progression objects
- Multiple objectives per quest

It is written in a way that it can be used for both, story games where you have a quest that you receive when talking to an NPC and finish once as well as for example "Monster Hunter"-like quests that need to be unlocked once and then can be repeated indefinitely.
Therefore you can see this project as a base building block for a quest system you want to make

# Installation
This plugin is made for Unreal Engine 5.2 and uses Unreals Subsystems to create a Quest Game Instance Subsystem.

1. Put the QuestSystem folder into your projects "Plugin"-folder
2. Start Unreal
3. Ensure it is enabled in the plugin tab

If it does not show up in the plugin tab you may add 
``` json
{
  "Name": "QuestSystem",
  "Enabled": true
}
```
to your projects .uproject file in the "Plugins" section.

The file should look at least like this when you install this plugin:
``` json
{
  "FileVersion": 3,
  "EngineAssociation": "5.2",
  "Category": "",
  "Description": "",
  "Modules": [
    {
      "Name": "ProjectName",
      "Type": "Runtime",
      "LoadingPhase": "Default",
      "AdditionalDependencies": [
      ]
    }
  ],
  "Plugins": [
    {
      "Name": "QuestSystem",
      "Enabled": true
    }
  ]
}
```

## Why are there basically no feature commits?
I made this plugin during another project in a private repository and ripped it out of there to have a standalone version.
