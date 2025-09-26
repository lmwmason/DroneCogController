from kivy.app import App
from kivy.uix.label import Label
from kivy.uix.boxlayout import BoxLayout
from kivy.uix.anchorlayout import AnchorLayout
from kivy.uix.button import Button
from kivy.uix.slider import Slider
from kivy.core.window import Window
from kivy.graphics import Color, Rectangle

from controller.moveBattery import move_battery

Window.clearcolor = (0.1, 0.1, 0.1, 1)
battery_mover = move_battery()

titleFont = './fonts/PartialSansKR-Regular.ttf'
posFont = './fonts/park.ttf'

cur_cog_pos=0
class MakeAllGui(App):
    global cur_cog_pos
    def build(self):
        main_layout = AnchorLayout(anchor_x='center', anchor_y='top')

        content_layout = BoxLayout(orientation='vertical',
                                   size_hint=(0.8, 0.6),
                                   padding=[20, 20, 20, 20],
                                   spacing=20)

        with content_layout.canvas.before:
            Color(0.2, 0.2, 0.2, 1)
            self.rect = Rectangle(pos=content_layout.pos, size=content_layout.size)

        content_layout.bind(pos=self.update_rect, size=self.update_rect)

        my_label = Label(text='드론 무게중심 관리', font_name=titleFont, font_size=40, color=(0.9, 0.9, 0.9, 1))

        self.value_label = Label(text='무게중심 앞뒤 거리: 0 mm',
                                 font_size=20,
                                 font_name = posFont,
                                 color=(0.9, 0.9, 0.9, 1))

        self.cog_slider = Slider(min=-100,
                                 max=100,
                                 value=0,
                                 step=1,
                                 size_hint_y=None,
                                 height=40,
                                 padding=10)

        self.cog_slider.bind(value=self.on_slider_value_change)

        confirm_button = Button(text='무게중심 맞추기',
                                font_name=titleFont,
                                size_hint_y=None,
                                height=60,
                                background_normal='',
                                background_color=(0.1, 0.7, 0.9, 1),
                                font_size=24)

        confirm_button.bind(on_press=self.get_slider_value)

        content_layout.add_widget(my_label)
        content_layout.add_widget(self.value_label)
        content_layout.add_widget(self.cog_slider)
        content_layout.add_widget(confirm_button)

        main_layout.add_widget(content_layout)

        return main_layout

    def update_rect(self, instance, value):
        self.rect.pos = instance.pos
        self.rect.size = instance.size

    def on_slider_value_change(self, instance, value):
        self.value_label.text = f'무게중심 앞뒤 거리: {int(value)} mm'

    def get_slider_value(self, instance):
        slider_value = self.cog_slider.value
        cur_cog_pos = int(self.cog_slider.value)
        battery_mover.goto_pos(cur_cog_pos)