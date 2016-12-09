-- Updates the hud with relevant information
class 'MainMenuHudSystem' (System)

--Names of the menus, used in MainMenu.layout
MAIN_MENU = "MainMenuInteractive"
OPTIONS_MENU = "OptionsMenuInteractive"
GRAPHICS_MENU = "GraphicsMenuInteractive"
SOUND_MENU = "SoundMenuInteractive"
CONTROLS_MENU = "ControlsMenuInteractive"
GAMEPLAY_MENU = "GameplayMenuInteractive"

gMainMenuHudSystem = nil

function MainMenuHudSystem:__init()
    System.__init(self)
end

function MainMenuHudSystem:setButtonBindings()
    if self.currentMenu == MAIN_MENU then
        if self.bindings[MAIN_MENU] == nil then
            --Main menu buttons
            self:bindButton("EditorMenuButton", mainMenuMicrobeEditorButtonClicked)
            self:bindButton("NewGameButton", mainMenuMicrobeStageButtonClicked)
            self:bindButton("LoadGameButton", mainMenuLoadButtonClicked)
            self:bindButton("ToolsButton", mainMenuToolsButtonClicked)
            self:bindButton("ExtrasButton", mainMenuExtrasButtonClicked)
            self:bindButton("CreditsButton", mainMenuCreditsButtonClicked)
            self:bindButton("OptionsButton", mainMenuOptionsButtonClicked)
            self:bindButton("ExitGameButton", quitButtonClicked) --defined in microbe_stage_hud.lua
            self.bindings[MAIN_MENU] = true
        end

    elseif self.currentMenu == OPTIONS_MENU then
        if self.bindings[OPTIONS_MENU] == nil then
            --Options menu buttons
            self:bindButton("GraphicOptionsButton", optionsMenuGraphicOptionsButtonClicked)
            self:bindButton("SoundOptionsButton", optionsMenuSoundOptionsButtonClicked)
            self:bindButton("GameplayOptionsButton", optionsMenuGameplayOptionsButtonClicked)
            self:bindButton("ControlsOptionsButton", optionsMenuControlsOptionsButtonClicked)
            self:bindButton("RevertSettingsButton", optionsMenuRevertSettingsButtonClicked)
            self:bindButton("MainMenuButton", optionsMenuMainMenuButtonClicked)
            self.bindings[OPTIONS_MENU] = true
        end
    end
end
    
function playHoverClickSound()
    local guiSoundEntity = Entity("gui_sounds")
    guiSoundEntity:getComponent(SoundSourceComponent.TYPE_ID):playSound("button-hover-click")
end

function MainMenuHudSystem:bindButton(name, buttonFunction)
    local button = root:getChild("Background"):getChild(self.currentMenu):getChild(name)
    button:registerEventHandler("Clicked", buttonFunction)
end

function MainMenuHudSystem:changeMenu(menu)
    --Hiding the current menu
    local menuWidget = root:getChild("Background"):getChild(self.currentMenu)
    menuWidget:hide()

    self.currentMenu = menu
    menuWidget = root:getChild("Background"):getChild(self.currentMenu)
    menuWidget:show()

    self:setButtonBindings()
end

function MainMenuHudSystem:init(gameState)
    self.bindings = {} --registers what menus are already binded to avoid repetition
    System.init(self, "MainMenuHudSystem", gameState)
    root = gameState:rootGUIWindow()

    self.currentMenu = MAIN_MENU
    self:setButtonBindings()
    self.bindings[MAIN_MENU] = true
  
	updateLoadButton();
    self.videoPlayer = CEGUIVideoPlayer("IntroPlayer")
    root:addChild( self.videoPlayer)
    self.hasShownIntroVid = false
    self.vidFadeoutStarted = false
    self.skippedVideo = false
    gMainMenuHudSystem = self
end

function MainMenuHudSystem:update(renderTime, logicTime)
    if keyCombo(kmp.screenshot) then
        Engine:screenShot("screenshot.png")
    elseif keyCombo(kmp.skipvideo) then
        if self.videoPlayer then
            self.videoPlayer:pause()
            self.videoPlayer:hide()
            Entity("gui_sounds"):getComponent(SoundSourceComponent.TYPE_ID):interruptPlaying()
            Entity("main_menu_ambience"):getComponent(SoundSourceComponent.TYPE_ID).autoLoop = true
            self.skippedVideo = true
        end
    elseif keyCombo(kmp.forward) then
    
    end
    if self.videoPlayer then
        self.videoPlayer:update()
        if self.videoPlayer:getCurrentTime() >= self.videoPlayer:getDuration() - 3.0 then
            if not self.vidFadeoutStarted then
                self.videoPlayer:playAnimation("fadeout")
                self.vidFadeoutStarted = true
            end
            if not self.skippedVideo and self.videoPlayer:getCurrentTime() >= self.videoPlayer:getDuration() then
                self.videoPlayer:hide()
                Entity("main_menu_ambience"):getComponent(SoundSourceComponent.TYPE_ID).autoLoop = true
            end
        end
    end
end

function MainMenuHudSystem:shutdown()
    -- Necessary to avoid failed assert in ogre on exit
    CEGUIVideoPlayer.destroyVideoPlayer(self.videoPlayer)
end

function MainMenuHudSystem:activate()
    updateLoadButton();
    if  self.videoPlayer and not self.hasShownIntroVid then
        self.videoPlayer:setVideo("intro.wmv")
        self.hasShownIntroVid = true
        self.videoPlayer:play()
    end
end
function updateLoadButton()
    if Engine:fileExists("quick.sav") then
        root:getChild("Background"):getChild("MainMenuInteractive"):getChild("LoadGameButton"):enable();
    else
        root:getChild("Background"):getChild("MainMenuInteractive"):getChild("LoadGameButton"):disable();
    end
end
