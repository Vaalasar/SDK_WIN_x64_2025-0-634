require 'sketchup.rb'
require 'extensions.rb'

module Examples
  module HelloOverlay

    unless file_loaded?(__FILE__)
      ex = SketchupExtension.new('Hello Overlay', 'ex_hello_c_overlay/main')
      ex.description = 'SketchUp Ruby API example using C API overlays.'
      ex.version     = '1.0.0'
      ex.copyright   = 'Trimble Inc © 2023'
      ex.creator     = 'SketchUp'
      Sketchup.register_extension(ex, true)
      file_loaded(__FILE__)
    end

  end
end
