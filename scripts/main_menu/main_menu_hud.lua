-- Updates the hud with relevant information
class 'MainMenuHudSystem' (System)

function MainMenuHudSystem:__init()
    System.__init(self)
end

function MainMenuHudSystem:init(gameState)
    System.init(self, gameState)
--NOGUI  root = gameState:rootGUIWindow()
--NOGUI  local microbeButton = root:getChild("Background"):getChild("MainMenuInteractive"):getChild("NewGameButton")
--NOGUI  local microbeEditorButton = root:getChild("Background"):getChild("MainMenuInteractive"):getChild("EditorMenuButton")
--NOGUI  local quitButton = root:getChild("Background"):getChild("MainMenuInteractive"):getChild("ExitGameButton")
--NOGUI  local loadButton = root:getChild("Background"):getChild("MainMenuInteractive"):getChild("LoadGameButton")   
--NOGUI  microbeButton:registerEventHandler("Clicked", mainMenuMicrobeStageButtonClicked)
--NOGUI  microbeEditorButton:registerEventHandler("Clicked", mainMenuMicrobeEditorButtonClicked)
--NOGUI  loadButton:registerEventHandler("Clicked", mainMenuLoadButtonClicked)
--NOGUI  quitButton:registerEventHandler("Clicked", quitButtonClicked)
--NOGUI  updateLoadButton();
end

function MainMenuHudSystem:update(renderTime, logicTime)
    if keyCombo(kmp.screenshot) then
        Engine:screenShot("screenshot.png")
    end
end

function MainMenuHudSystem:activate()
    updateLoadButton();
end
function updateLoadButton()
--NOGUI    if Engine:fileExists("quick.sav") then
--NOGUI        root:getChild("Background"):getChild("MainMenuInteractive"):getChild("LoadGameButton"):enable();
--NOGUI    else
--NOGUI        root:getChild("Background"):getChild("MainMenuInteractive"):getChild("LoadGameButton"):disable();
--NOGUI    end
end

function mainMenuLoadButtonClicked()
    local guiSoundEntity = Entity("gui_sounds")
    guiSoundEntity:getComponent(SoundSourceComponent.TYPE_ID):playSound("button-hover-click")
    Engine:setCurrentGameState(GameState.MICROBE)
    Engine:load("quick.sav")
    print("Game loaded");
end

function mainMenuMicrobeStageButtonClicked()
    local guiSoundEntity = Entity("gui_sounds")
    guiSoundEntity:getComponent(SoundSourceComponent.TYPE_ID):playSound("button-hover-click")
    Engine:setCurrentGameState(GameState.MICROBE)
end

function mainMenuMicrobeEditorButtonClicked()
    local guiSoundEntity = Entity("gui_sounds")
    guiSoundEntity:getComponent(SoundSourceComponent.TYPE_ID):playSound("button-hover-click")
    Engine:setCurrentGameState(GameState.MICROBE_EDITOR)
end

-- quitButtonClicked is already defined in microbe_stage_hud.lua
