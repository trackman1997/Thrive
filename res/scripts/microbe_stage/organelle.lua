class 'Organelle'

function Organelle:__init()
    self.entity = Entity()
    self.entity:setVolatile(true)
    self.sceneNode = self.entity:getOrCreate(OgreSceneNodeComponent)
    self.collisionShape = CompoundShape()
    self._hexes = {}
    self.position = {
        q = 0,
        r = 0
    }
    self._colour = ColourValue(1,1,1,1)
    self._internalEdgeColour = ColourValue(0.5, 0.5, 0.5, 1)
    self._externalEdgeColour = ColourValue(0, 0, 0, 1)
    self._needsColourUpdate = false
end


function Organelle:addHex(q, r)
    assert(not self.microbe, "Cannot change organelle shape while it is in a microbe")
    local s = encodeAxial(q, r)
    if self._hexes[s] then
        return false
    end
    local hex = {
        q = q,
        r = r,
        entity = Entity(),
        collisionShape = SphereShape(HEX_SIZE),
        sceneNode = OgreSceneNodeComponent()
    }
    local x, y = axialToCartesian(q, r)
    local translation = Vector3(x, y, 0)
    hex.entity:setVolatile(true)
    -- Scene node
    hex.sceneNode.parent = self.entity
    hex.sceneNode.transform.position = translation
    hex.sceneNode.transform:touch()
    hex.sceneNode.meshName = "hex.mesh"
    hex.entity:addComponent(hex.sceneNode)
    -- Collision shape
    self.collisionShape:addChildShape(
        translation,
        Quaternion(Radian(0), Vector3(1,0,0)),
        hex.collisionShape
    )
    self._hexes[s] = hex
    return true
end


function Organelle:getHex(q, r)
    local s = encodeAxial(q, r)
    return self._hexes[s]
end


function Organelle:load(storage)
    local hexes = storage:get("hexes", {})
    for i = 1,hexes:size() do
        local hexStorage = hexes:get(i)
        local q = hexStorage:get("q", 0)
        local r = hexStorage:get("r", 0)
        self:addHex(q, r)
    end
    self.position.q = storage:get("q", 0)
    self.position.r = storage:get("r", 0)
    self._colour = storage:get("colour", ColourValue.White)
    self._internalEdgeColour = storage:get("internalEdgeColour", ColourValue.Grey)
    self._externalEdgeColour = storage:get("externalEdgeColour", ColourValue.Black)
end


function Organelle.loadOrganelle(storage)
    local className = storage:get("className", "")
    local cls = _G[className]
    local organelle = cls()
    organelle:load(storage)
    return organelle
end


function Organelle:onAddedToMicrobe(microbe, q, r)
    self.microbe = microbe
    self.position.q = q
    self.position.r = r
end


function Organelle:onRemovedFromMicrobe(microbe)
    assert(microbe == self.microbe, "Can't remove organelle, wrong microbe")
    self.microbe = nil
    self.position.q = 0
    self.position.r = 0
end


function Organelle:removeHex(q, r)
    assert(not self.microbe, "Cannot change organelle shape while it is in a microbe")
    local s = encodeAxial(q, r)
    local hex = table.remove(self._hexes, s)
    if hex then
        hex.entity:destroy()
        self.collisionShape:removeChildShape(hex.collisionShape)
        return true
    else
        return false
    end
end


function Organelle:setColour(colour)
    self._colour = colour
    self._needsColourUpdate = true
end


function Organelle:storage()
    storage = StorageContainer()
    storage:set("className", class_info(self).name)
    hexes = StorageList()
    for _, hex in pairs(self._hexes) do
        hexStorage = StorageContainer()
        hexStorage:set("q", hex.q)
        hexStorage:set("r", hex.r)
        hexes:append(hexStorage)
    end
    storage:set("hexes", hexes)
    storage:set("q", self.position.q)
    storage:set("r", self.position.r)
    storage:set("colour", self._colour)
    storage:set("internalEdgeColour", self._internalEdgeColour)
    storage:set("externalEdgeColour", self._externalEdgeColour)
    return storage
end


function Organelle:update(microbe, milliseconds)
    if self._needsColourUpdate then
        self:_updateHexColours()
    end
    -- Nothing
end


function Organelle:_updateHexColours()
    for _, hex in pairs(self._hexes) do
        if not hex.sceneNode.entity then
            self._needsColourUpdate = true
            return
        end
        local center = hex.sceneNode.entity:getSubEntity("center")
        center:setMaterial(getColourMaterial(self._colour))
        for i, qs, rs in iterateNeighbours(hex.q, hex.r) do
            local neighbourHex = self:getHex(qs, rs)
            local neighbourOrganelle = self.microbe and self.microbe:getOrganelleAt(
                self.position.q + qs,
                self.position.r + rs
            )
            local sideName = HEX_SIDE_NAME[i]
            local subEntity = hex.sceneNode.entity:getSubEntity(sideName)
            local edgeColour = nil
            if neighbourHex then
                edgeColour = self._colour
            elseif neighbourOrganelle then
                edgeColour = self._internalEdgeColour
            else
                edgeColour = self._externalEdgeColour
            end
            subEntity:setMaterial(getColourMaterial(edgeColour))
        end
    end
    self._needsColourUpdate = false
end

function Organelle:updateHexColours()
    self._needsColourUpdate = true
end