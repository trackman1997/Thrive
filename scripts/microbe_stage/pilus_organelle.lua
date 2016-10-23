--------------------------------------------------------------------------------
-- Class for the predatory pilus organelle
--------------------------------------------------------------------------------
class 'PilusOrganelle' (Organelle)

-- Constructor
--
-- @param baseDamage
--  The base damage this organelle produces before speed multipliers
--
-- @param speedMultiplier
--  Damage produced based on the relative speed between the microbes
--
-- @param minSpeed
--  Minimum speed necesary to register a hit (to avoid exploits?)
--
-- @param translation
--  Where the organelle goes in the microbe
function PilusOrganelle:__init(baseDamage, speedMultiplier, minSpeed)
    Organelle.__init(self)
    self.baseDamage = baseDamage
    self.speedMultiplier = speedMultiplier
    self.minSpeed = minSpeed
end

function PilusOrganelle:onAddedToMicrobe(microbe, q, r, rotation)  
    Organelle.onAddedToMicrobe(self, microbe, q, r, rotation)
end

function PilusOrganelle:load(storage)
    Organelle.load(self, storage)
    self.baseDamage = storage:get("baseDamage", 0.025)
    self.speedMultiplier = storage:get("speedMultiplier", 0.025)
    self.minSpeed = storage:get("minSpeed", 0.025)
end

function PilusOrganelle:storage()
    local storage = Organelle.storage(self)
    storage:set("baseDamage", self.baseDamage)
    storage:set("speedMultiplier", self.speedMultiplier)
    storage:set("minSpeed", self.minSpeed)
    return storage
end

function PilusOrganelle:update(microbe, logicTime)
    local x, y = axialToCartesian(self.position.q, self.position.r)
    local membraneCoords = microbe.membraneComponent:getExternOrganellePos(x, y)
    local translation = Vector3(membraneCoords[1], membraneCoords[2], 0)
    local position = translation - Vector3(x, y, 0)
    self.organelleEntity.sceneNode.transform.position = position
    self.organelleEntity.sceneNode.transform:touch()
end

Organelle.mpCosts["pilus"] = 25

-- factory functions
function OrganelleFactory.make_pilus(data)
    local pilus = PilusOrganelle(15, 2, 1) -- fine-tune this values
    pilus:addHex(0, 0)
    return pilus
end

function OrganelleFactory.render_pilus(data)
	local x, y = axialToCartesian(data.q, data.r)
	local translation = Vector3(-x, -y, 0)
	data.sceneNode[1].meshName = "pilus.mesh"
	data.sceneNode[1].transform.position = translation
	data.sceneNode[1].transform.orientation = Quaternion(Radian(Degree(-90)), Vector3(0, 0, 1))
	
	data.sceneNode[2].transform.position = translation
	OrganelleFactory.setColour(data.sceneNode[2], data.colour)
end

function OrganelleFactory.sizeof_pilus(data)
    local hexes = {}
	hexes[1] = {["q"]=0, ["r"]=0}
	return hexes
end