--[[ PUBLIC MEMBERS

.init()
.graphics(graphic)
.collection(graphics)
.absolute(image, x, y)
.text(text)
.textCollection(texts)
.isMouseInBox(x, y, width, height)
.isMouseOverGraphic(graphic)
.scaleUp()
.scaleDown()

]]

local draw = {
  debug1 = '',
  debug2 = ''
}
local private = {}

local scaleFactor = 3
local aspectCorrection = 1.2

function draw.init()
  love.graphics.setDefaultFilter('nearest', 'nearest')
  private.resetWindow()
end

function draw.clickItemIn(graphics, x, y, button)
  local clicked = false

  for k,v in pairs(graphics) do
    if v.clicked and draw.isOverGraphic(v, x, y) then
      v:clicked(button)
      clicked = true
      break
    end
  end

  return clicked
end

function draw.graphics(graphic)
  local a = graphic.anim
  local h = graphic.hover
  local i = get(graphic.active) or 1

  local image = graphic.assets[i]

  private.drawImage(image, graphic.x, graphic.y)

  if a and graphic.timer then
    private.drawImage(graphic.assets[a], graphic.x, graphic.y)
  elseif h then
    local inBox = draw.isMouseInBox(graphic.x, graphic.y, image:getWidth(), image:getHeight())

    if inBox then
      private.drawImage(graphic.assets[h], graphic.x, graphic.y)
    end
  end

end

function draw.collection(graphics)
  for k,v in pairs(graphics) do
    if type(v) == 'table' then
      draw.graphics(v)
    end
  end
end

function draw.absolute(image, x, y)
  love.graphics.draw(image, x, y, 0, scaleFactor, scaleFactor * aspectCorrection)
end

function draw.text(text)
  love.graphics.setFont(get(text.font))

  if text.w then
    love.graphics.printf(
      text.value, 
      text.x * scaleFactor, 
      text.y * scaleFactor, 
      text.w * scaleFactor, 
      text.align, 
      0, 
      scaleFactor, --/ aspectCorrection, 
      scaleFactor --* aspectCorrection
    )
  else
    love.graphics.print(
      text.value, 
      text.x * scaleFactor, 
      text.y * scaleFactor, 
      0, 
      scaleFactor, --/ aspectCorrection, 
      scaleFactor --* aspectCorrection
    )
  end
end

function draw.textCollection(texts)
  for k,v in pairs(texts) do
    if type(v) == 'table' then
      draw.text(v)
    end
  end
end

function private.drawImage(image, x, y)
  love.graphics.draw(image, x * scaleFactor, y * scaleFactor * aspectCorrection, 0, scaleFactor, scaleFactor * aspectCorrection)
end

function draw.isMouseInBox(x, y, width, height)
  local mx = love.mouse.getX()
  local my = love.mouse.getY()
  
  return private.isInBox(mx, my, x, y, width, height, true)
end

function draw.isOverGraphic(graphic, mx, my)
  local act = graphic['active'] or 1
  local w = graphic.assets[act]:getWidth()
  local h = graphic.assets[act]:getHeight()
  return private.isInBox(mx, my, graphic.x, graphic.y, w, h, true)
end

function private.isInBox(testX, testY, boxX, boxY, boxWidth, boxHeight, correctAspect)
  local westX = boxX * scaleFactor
  local northY = boxY * scaleFactor * (correctAspect and aspectCorrection or 1)
  local eastX = (boxX + boxWidth) * scaleFactor
  local southY = (boxY + boxHeight) * scaleFactor * (correctAspect and aspectCorrection or 1)

  return westX < testX and testX <= eastX and northY < testY and testY <= southY
end

function draw.scaleUp()
  scaleFactor = scaleFactor > 4 and 5 or scaleFactor + 1
  private.resetWindow()
end

function draw.scaleDown()
  scaleFactor = scaleFactor < 2 and 1 or scaleFactor - 1
  private.resetWindow()
end

function draw.debug()
  love.graphics.print(draw.debug1, 10 * scaleFactor, 185 * scaleFactor * aspectCorrection)
  love.graphics.print(draw.debug2, 10 * scaleFactor, 190 * scaleFactor * aspectCorrection)
end

function private.resetWindow()
  love.window.setMode(320 * scaleFactor, 200 * scaleFactor * aspectCorrection)
end

return draw