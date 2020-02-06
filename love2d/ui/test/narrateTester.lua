local narrateTester = {}
-- This loads the C library and binds is to a 'ds' object
--ds = require 'libds'
--dsl = {}

ADD_MENU = 0
PORTRAIT = 1
SHOW_TEXT = 2
SHOW_MENU = 3
EDIT_BOX = 4

-- When narrate_open is called it either creates an dialog box OR edits the current existing one.
-- It is VERY common for narrate_open to be called with PORTRAIT and no text, then have SHOW_TEXT later
-- with possibly a menu.
-- NOTE: SHOW_TEXT may be (and usually is) called multiple times on the same dialog.
function dsl.narrate_open2(action, text, index)
    if (action == ADD_MENU) then
        -- Here you add a menu item (text).
        -- AKA: add an option for the player to select
    end
    if (action == PORTRAIT) then
        -- Here set the portrait by index.
        data, width, height = ds.get_portrait(index)
        if (data ~=  0) then -- always check that it got the image!
            imageData = love.image.newImageData(width, height, "rgba8", data)
            portrait_image = love.graphics.newImage( imageData )
        end
        -- Now portrait_image *should* be the image to display
        -- Also, if text is not empty, then text needs to be displayed as well.
    end
    if (text) then --(action == SHOW_TEXT) then
        -- Here display the message text to the user
        _text = text
    end
    if (action == SHOW_MENU) then
        -- Display the menu
    end
    print("I'm in LUA!");
end

function narrateTester.run()
    -- Warning: This currently only works once!  (don't call again for now...)
    ds.mas_execute(42) -- Change to the arena region

    -- When everything is setup the following functions are triggered by some event, but that
    -- isn't implemented right now.  Instead we trigger manually by calling
    -- dsl_execute.

    -- This one creates the first dialog and then exits (prematurely.)
    -- I need to come up with a scheme of how to continue the gpl/dsl script.
    -- So that once the player responds the conversation can continue.
    ds.dsl_execute(5, 2348) -- execute the dialog/narration
    ds.select_menu(0);      -- Select menu item #0 (#1, and #2 are available as well.)

    -- These produce dialogs:
    --ds.dsl_execute(5, 931)
    --ds.dsl_execute(4, 1) -- I never got this in the real game, did you?
    --ds.dsl_execute(5, 772)

    --LOOK CHECKS that produce dialogs:
    --ds.dsl_execute(5, 6792)
    --ds.dsl_execute(5, 6832)

    -- These appear to work, but don't do anything right now.  Some flags/objects probably need to exist first...
    --ds.dsl_execute(2, 1)
    --ds.dsl_execute(5, 54) -- lays a trap
    --ds.dsl_execute(5, 74) -- lays a trap
    --ds.dsl_execute(5, 94) -- lays a trap
    --ds.dsl_execute(5, 114) -- lays a trap
    --ds.dsl_execute(3, 1854) -- Seems the same as (5, 6530)
    --ds.dsl_execute(5, 6530) -- Seems the same as (3, 1854)
    print ("END NARRARTE TEST")
end

return narrateTester
