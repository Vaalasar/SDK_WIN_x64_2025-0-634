require 'sketchup.rb'
require 'fiddle'
require 'fiddle/import'

module Examples
  module HelloOverlay

    module HelloCOverlay
      extend Fiddle::Importer
      names = { :platform_win => 'ex_hello_overlay.dll', :platform_osx => 'libex_hello_overlay.so'}
      name = names[Sketchup.platform]
      dlload("#{File.expand_path(File.dirname(__FILE__))}/bin/Debug/#{name}")
      extern('void RegisterImageOverlay(const char*)')
    end

    unless file_loaded?(__FILE__)
      menu = UI.menu('Extensions')
      menu.add_item('Overlay Image') {
        chosen_image = UI.openpanel('Select an image file for overlay', nil, 'Image Files|*.jpg;*.png;||')
        if chosen_image
          HelloCOverlay::RegisterImageOverlay(chosen_image)
        end
      }
      file_loaded(__FILE__)
    end

  end
end # module Examples
