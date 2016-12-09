function mainMenuLoadButtonClicked()
    playHoverClickSound()
    Engine:setCurrentGameState(GameState.MICROBE)
    Engine:load("quick.sav")
    print("Game loaded");
end

function mainMenuMicrobeStageButtonClicked()
    playHoverClickSound()
    Engine:setCurrentGameState(GameState.MICROBE_TUTORIAL)
end

function mainMenuMicrobeEditorButtonClicked()
    playHoverClickSound()
    Engine:setCurrentGameState(GameState.MICROBE_EDITOR)
end

function mainMenuOptionsButtonClicked()
    playHoverClickSound()
    gMainMenuHudSystem:changeMenu(OPTIONS_MENU)
end

function mainMenuToolsButtonClicked()
    playHoverClickSound()
end

function mainMenuExtrasButtonClicked()
    playHoverClickSound()
end

function mainMenuCreditsButtonClicked()
    playHoverClickSound()
end
