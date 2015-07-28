
-- Updates the hud with relevant information
class 'HudSystem' (System)

function HudSystem:__init()
    System.__init(self)
	self.compoundListBox = nil
	self.hitpointsCountLabel = nil
	self.hitpointsBar = nil
	self.compoundListItems = {}
    self.rootGuiWindow = nil
    self.populationNumberLabel = nil
    self.rootGUIWindow = nil
end

global_if_already_displayed = false

function HudSystem:activate()
  --NOGUI  global_activeMicrobeStageHudSystem = self -- Global reference for event handlers
  --NOGUI  lockedMap = Engine:playerData():lockedMap()
  --NOGUI  if lockedMap ~= nil and not lockedMap:isLocked("Toxin") and not ss and not global_if_already_displayed then
  --NOGUI      showMessage("'E' Releases Toxin")
  --NOGUI      global_if_already_displayed = true
  --NOGUI  end
  --NOGUI  self.helpOpen = false
  --NOGUI  self.menuOpen = true
  --NOGUI  self:updateLoadButton();
end

function HudSystem:init(gameState)
    System.init(self, gameState) ---[[
  --NOGUI  self.rootGUIWindow =  gameState:rootGUIWindow()
  --NOGUI  self.compoundListBox = self.rootGUIWindow:getChild("CompoundsOpen"):getChild("CompoundsLabel")
  --NOGUI  self.hitpointsBar = self.rootGUIWindow:getChild("HealthPanel"):getChild("LifeBar")
  --NOGUI  self.hitpointsCountLabel = self.hitpointsBar:getChild("NumberLabel")
  --NOGUI  self.nameLabel = self.rootGUIWindow:getChild("SpeciesNamePanel"):getChild("SpeciesNameLabel")
  --NOGUI  local menuButton = self.rootGUIWindow:getChild("MenuButton")
  --NOGUI  local saveButton = self.rootGUIWindow:getChild("SaveGameButton") 
  --NOGUI  local loadButton = self.rootGUIWindow:getChild("LoadGameButton")	
  --NOGUI  --local collapseButton = self.rootGUIWindow:getChild() collapseButtonClicked
  --NOGUI  local helpButton = self.rootGUIWindow:getChild("HelpButton")
  --NOGUI  self.editorButton = self.rootGUIWindow:getChild("EditorButton")
  --NOGUI  --local returnButton = self.rootGUIWindow:getChild("MenuButton")
  --NOGUI  local compoundButton = self.rootGUIWindow:getChild("CompoundsClosed")
  --NOGUI  local compoundPanel = self.rootGUIWindow:getChild("CompoundsOpen")
  --NOGUI  local quitButton = self.rootGUIWindow:getChild("QuitButton")
  --NOGUI  saveButton:registerEventHandler("Clicked", function() self:saveButtonClicked() end)
  --NOGUI  loadButton:registerEventHandler("Clicked", function() self:loadButtonClicked() end)
  --NOGUI  menuButton:registerEventHandler("Clicked", function() self:menuButtonClicked() end)
  --NOGUI  helpButton:registerEventHandler("Clicked", function() self:helpButtonClicked() end)
  --NOGUI  self.editorButton:registerEventHandler("Clicked", function() self:editorButtonClicked() end)
  --NOGUI  --returnButton:registerEventHandler("Clicked", returnButtonClicked)
  --NOGUI  compoundButton:registerEventHandler("Clicked", function() self:openCompoundPanel() end)
  --NOGUI  compoundPanel:registerEventHandler("Clicked", function() self:closeCompoundPanel() end)
  --NOGUI  quitButton:registerEventHandler("Clicked", quitButtonClicked)
  --NOGUI  self.rootGUIWindow:getChild("MainMenuButton"):registerEventHandler("Clicked", menuMainMenuClicked) -- in microbe_editor_hud.lua
  --NOGUI  self:updateLoadButton();
end


function HudSystem:update(renderTime)
    local player = Entity("player")
    local playerMicrobe = Microbe(player)
    self.nameLabel:setText(playerMicrobe.microbe.speciesName)

 --NOGUI   self.hitpointsBar:progressbarSetProgress(playerMicrobe.microbe.hitpoints/playerMicrobe.microbe.maxHitpoints)
 --NOGUI   self.hitpointsCountLabel:setText("".. math.floor(playerMicrobe.microbe.hitpoints))
 --NOGUI   local playerSpecies = playerMicrobe:getSpeciesComponent()
 --NOGUI   --TODO display population in home patch here
 --NOGUI   for compoundID in CompoundRegistry.getCompoundList() do
 --NOGUI       local compoundsString = string.format("%s - %d", CompoundRegistry.getCompoundDisplayName(compoundID), playerMicrobe:getCompoundAmount(compoundID))
 --NOGUI       if self.compoundListItems[compoundID] == nil then
 --NOGUI          -- TODO: fix this colour
 --NOGUI          self.compoundListItems[compoundID] = StandardItemWrapper("[colour='FF004400']" .. compoundsString, compoundID)
 --NOGUI          -- The object will be deleted by CEGUI so make sure that it isn't touched after destroying the layout
 --NOGUI          self.compoundListBox:listWidgetAddItem(self.compoundListItems[compoundID])
 --NOGUI       else
 --NOGUI          self.compoundListBox:listWidgetUpdateItem(self.compoundListItems[compoundID],
 --NOGUI                                                     "[colour='FF004400']" .. compoundsString)
 --NOGUI       end
 --NOGUI   end
    
    
    if keyCombo(kmp.togglemenu) then
        self:menuButtonClicked()
    elseif keyCombo(kmp.gotoeditor) then
        self:editorButtonClicked()
    elseif keyCombo(kmp.shootoxytoxy) then
        playerMicrobe:emitAgent(CompoundRegistry.getCompoundId("oxytoxy"), 3)
    elseif keyCombo(kmp.reproduce) then
        playerMicrobe:reproduce()
    end
    local direction = Vector3(0, 0, 0)
    if keyCombo(kmp.forward) then
        playerMicrobe.soundSource:playSound("microbe-movement-2")
    end
    if keyCombo(kmp.backward) then
        playerMicrobe.soundSource:playSound("microbe-movement-2")
    end
    if keyCombo(kmp.leftward) then
        playerMicrobe.soundSource:playSound("microbe-movement-1")
    end
    if keyCombo(kmp.screenshot) then
        Engine:screenShot("screenshot.png")
    end
    if keyCombo(kmp.rightward) then
        playerMicrobe.soundSource:playSound("microbe-movement-1")
    end
    if (Engine.keyboard:wasKeyPressed(Keyboard.KC_G)) then
        playerMicrobe:toggleEngulfMode()
    end
    
    offset = Entity(CAMERA_NAME):getComponent(OgreCameraComponent.TYPE_ID).properties.offset
    newZVal = offset.z + Engine.mouse:scrollChange()/10
    if newZVal < 10 then
        newZVal = 10
    elseif newZVal > 80 then
        newZVal = 80
    end
    offset.z = newZVal --]]
end

function showReproductionDialog() global_activeMicrobeStageHudSystem:showReproductionDialog() end

function HudSystem:showReproductionDialog()
   -- print("Reproduction Dialog called but currently disabled. Is it needed? Note that the editor button has been enabled")
    --global_activeMicrobeStageHudSystem.rootGUIWindow:getChild("ReproductionPanel"):show()
    self.editorButton:enable()
end

function showMessage(msg)
    print(msg.." (note, in-game messages currently disabled)")
    --local messagePanel = Engine:currentGameState():rootGUIWindow():getChild("MessagePanel")
    --messagePanel:getChild("MessageLabel"):setText(msg)
    --messagePanel:show()
end

function HudSystem:updateLoadButton()
  --NOGUI  if Engine:fileExists("quick.sav") then
  --NOGUI      self.rootGUIWindow:getChild("LoadGameButton"):enable();
  --NOGUI  else
  --NOGUI      self.rootGUIWindow:getChild("LoadGameButton"):disable();
  --NOGUI  end
end

--Event handlers
function HudSystem:saveButtonClicked()
    local guiSoundEntity = Entity("gui_sounds")
    guiSoundEntity:getComponent(SoundSourceComponent.TYPE_ID):playSound("button-hover-click")
    Engine:save("quick.sav")
    print("Game Saved");
	--Because using update load button here doesn't seem to work unless you press save twice
  --NOGUI  self.rootGUIWindow:getChild("LoadGameButton"):enable();
end
function HudSystem:loadButtonClicked()
    local guiSoundEntity = Entity("gui_sounds")
    guiSoundEntity:getComponent(SoundSourceComponent.TYPE_ID):playSound("button-hover-click")
    Engine:load("quick.sav")
    print("Game loaded");
end

function HudSystem:menuButtonClicked()
  --NOGUI  local guiSoundEntity = Entity("gui_sounds")
  --NOGUI  guiSoundEntity:getComponent(SoundSourceComponent.TYPE_ID):playSound("button-hover-click")
  --NOGUI  print("played sound")
  --NOGUI  if not self.menuOpen then
  --NOGUI      self.rootGUIWindow:getChild("StatsButton"):playAnimation("MoveToStatsButton");
  --NOGUI      self.rootGUIWindow:getChild("HelpButton"):playAnimation("MoveToHelpButton");
  --NOGUI      self.rootGUIWindow:getChild("OptionsButton"):playAnimation("MoveToOptionsButton");
  --NOGUI      self.rootGUIWindow:getChild("LoadGameButton"):playAnimation("MoveToLoadGameButton");
  --NOGUI      self.rootGUIWindow:getChild("SaveGameButton"):playAnimation("MoveToSaveGameButton");
  --NOGUI      self:updateLoadButton();
  --NOGUI      self.menuOpen = true
  --NOGUI  else
  --NOGUI      self.rootGUIWindow:getChild("StatsButton"):playAnimation("MoveToMenuButtonD0");
  --NOGUI      self.rootGUIWindow:getChild("HelpButton"):playAnimation("MoveToMenuButtonD2");
  --NOGUI      self.rootGUIWindow:getChild("OptionsButton"):playAnimation("MoveToMenuButtonD1");
  --NOGUI      self.rootGUIWindow:getChild("LoadGameButton"):playAnimation("MoveToMenuButtonD4");
  --NOGUI      self.rootGUIWindow:getChild("SaveGameButton"):playAnimation("MoveToMenuButtonD3");
  --NOGUI      self:updateLoadButton();
  --NOGUI      self.menuOpen = false
  --NOGUI  end
end


function HudSystem:openCompoundPanel()
    local guiSoundEntity = Entity("gui_sounds")
  --NOGUI  guiSoundEntity:getComponent(SoundSourceComponent.TYPE_ID):playSound("button-hover-click")
  --NOGUI  self.rootGUIWindow:getChild("CompoundsOpen"):show()
  --NOGUI  self.rootGUIWindow:getChild("CompoundsClosed"):hide()
end

function HudSystem:closeCompoundPanel()
    local guiSoundEntity = Entity("gui_sounds")
    guiSoundEntity:getComponent(SoundSourceComponent.TYPE_ID):playSound("button-hover-click")
   --NOGUI self.rootGUIWindow:getChild("CompoundsOpen"):hide()
   --NOGUI self.rootGUIWindow:getChild("CompoundsClosed"):show()
end

function HudSystem:helpButtonClicked()
   --NOGUI local guiSoundEntity = Entity("gui_sounds")
   --NOGUI guiSoundEntity:getComponent(SoundSourceComponent.TYPE_ID):playSound("button-hover-click")
   --NOGUI --Engine:currentGameState():rootGUIWindow():getChild("MenuPanel"):hide()
   --NOGUI if Engine:currentGameState():name() == "microbe" then
   --NOGUI     if self.helpOpen then
   --NOGUI         Engine:resumeGame()
   --NOGUI         self.rootGUIWindow:getChild("HelpPanel"):hide()
   --NOGUI     else
   --NOGUI         Engine:pauseGame()
   --NOGUI         self.rootGUIWindow:getChild("HelpPanel"):show()
   --NOGUI     end
   --NOGUI     self.helpOpen = not self.helpOpen
   --NOGUI end
end


function HudSystem:editorButtonClicked()
    local guiSoundEntity = Entity("gui_sounds")
    guiSoundEntity:getComponent(SoundSourceComponent.TYPE_ID):playSound("button-hover-click")
   --NOGUI self.editorButton:disable()
    Engine:setCurrentGameState(GameState.MICROBE_EDITOR)
end

--[[
function HudSystem:returnButtonClicked()
    local guiSoundEntity = Entity("gui_sounds")
    guiSoundEntity:getComponent(SoundSourceComponent.TYPE_ID):playSound("button-hover-click")
    --Engine:currentGameState():rootGUIWindow():getChild("MenuPanel"):hide()
    if Engine:currentGameState():name() == "microbe" then
        Engine:currentGameState():rootGUIWindow():getChild("HelpPanel"):hide()
        Engine:currentGameState():rootGUIWindow():getChild("MessagePanel"):hide()
        Engine:currentGameState():rootGUIWindow():getChild("ReproductionPanel"):hide()
        Engine:resumeGame()
    elseif Engine:currentGameState():name() == "microbe_editor" then
        Engine:currentGameState():rootGUIWindow():getChild("SaveLoadPanel"):hide()
    end
end --]]

function quitButtonClicked()
    local guiSoundEntity = Entity("gui_sounds")
    guiSoundEntity:getComponent(SoundSourceComponent.TYPE_ID):playSound("button-hover-click")
    Engine:quit()
end
