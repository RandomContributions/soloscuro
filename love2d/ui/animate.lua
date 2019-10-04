local animate = {}
local private = {}

function animate.updateCollection(dt, graphics)
  for k,v in pairs(graphics) do
    animate.update(dt, v)
  end
end

function animate.update(dt, graphic)
  if graphic.animation and graphic.timer then
    graphic.timer = graphic.timer + dt
    local imgIndex = 1

    for i = 1, #graphic.animation do
      if graphic.timer < (i * graphic.animation.interval) then
        imgIndex = graphic.animation[i]
        break
      end
    end

    graphic.anim = imgIndex

    if graphic.timer > (#graphic.animation * graphic.animation.interval) then
      graphic.timer = nil
      graphic.anim = nil

      if graphic.animComplete then
        graphic:animComplete()
      end
    end
  end
end

return animate