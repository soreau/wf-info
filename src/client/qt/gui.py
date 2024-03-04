from PyQt5.QtWidgets import QApplication, QMainWindow, QLabel, QPushButton, QHBoxLayout, QVBoxLayout, QWidget
from wf_info_base import WfInfoBase
from pywayland.client import Display
from PyQt5.QtCore import Qt
import signal
import sys

class WfInfoApp(QMainWindow):
    def __init__(self, wf_info, clipboard):
        super().__init__()
        self.wf_info = wf_info
        self.clipboard = clipboard

        self.setWindowTitle("Wayfire Window Information")
        self.setGeometry(100, 100, 400, 500)

        self.central_widget = QWidget()
        self.setCentralWidget(self.central_widget)

        self.info_button = QPushButton("Click to get window information")

        self.layout = QVBoxLayout()

        self.layout.addWidget(self.info_button)

        self.central_widget.setLayout(self.layout)

        self.info_button.clicked.connect(self.get_info_text)
    def reset_layout(self, layout):
        if layout is not None:
            while layout.count():
                item = layout.takeAt(0)
                widget = item.widget()
                if widget is not None:
                    widget.setParent(None)
                else:
                    self.reset_layout(item.layout())
    def reset(self):
        self.reset_layout(self.layout)
        self.layout.addWidget(self.info_button)
    def copy_text(self, text):
        self.clipboard.setText(text)
    def handle_view_info(self, ok, view_id, client_pid, workspace_x, workspace_y, app_id, title, role, x, y, width, height, is_xwayland, focused, output, output_id):
        self.reset()
        labels = ["LABEL", "view_id", "client_pid", "workspace_x", "workspace_y", "app_id", "title", "role", "x", "y", "width", "height", "is_xwayland", "focused", "output", "output_id"]
        values = ["VALUE", f"{view_id}", f"{client_pid}", f"{workspace_x}", f"{workspace_y}", f"{app_id}", f"{title}", f"{role}", f"{x}", f"{y}", f"{width}", f"{height}", f"{is_xwayland}", f"{focused}", f"{output}", f"{output_id}"]
        hlayout = QHBoxLayout()
        label_layout = QVBoxLayout()
        i = 0
        for label in labels:
            i = i + 1
            if i == 1:
                label_label = QLabel(label)
            else:
                label_label = QLabel(label + ": ")
            label_label.setTextInteractionFlags(Qt.TextSelectableByMouse)
            label_layout.addWidget(label_label)
        hlayout.addLayout(label_layout)
        value_layout = QVBoxLayout()
        for value in values:
            value_label = QLabel(value)
            value_label.setTextInteractionFlags(Qt.TextSelectableByMouse)
            value_layout.addWidget(value_label)
        hlayout.addLayout(value_layout)
        copy_button_layout = QVBoxLayout()
        i = 0
        for value in values:
            i = i + 1
            if i == 1:
                reset_button = QPushButton("Reset")
                reset_button.clicked.connect(self.reset)
                copy_button_layout.addWidget(reset_button)
                continue
            copy_button = QPushButton("Copy Value")
            copy_button.clicked.connect(lambda s=self, v=value: self.copy_text(v))
            copy_button_layout.addWidget(copy_button)
        hlayout.addLayout(copy_button_layout)

        self.layout.addLayout(hlayout)
        self.layout.addWidget(self.info_button)
    def get_info_text(self):
        self.wf_info["binding"].dispatcher["view_info"] = self.handle_view_info
        self.wf_info["binding"].view_info()
        self.wf_info["display"].dispatch(block=True)


def handle_registry_global(wl_registry, id_num, iface_name, version):
    #print("global", id_num, iface_name)

    wf_info = wl_registry.user_data
    if iface_name == "wf_info_base":
        wf_info["enabled"] = True
        wf_info["binding"] = wl_registry.bind(id_num, WfInfoBase, 1)
    return 1

def wf_info_create():
    wf_info = {}

    # Make the display and get the registry
    wf_info["display"] = Display()
    wf_info["display"].connect()

    wf_info["registry"] = wf_info["display"].get_registry()
    wf_info["registry"].user_data = wf_info
    wf_info["registry"].dispatcher["global"] = handle_registry_global

    wf_info["enabled"] = False
    wf_info["display"].roundtrip()

    if not wf_info["enabled"]:
        print("Wayfire information protocol not advertised by compositor. Is wf-info plugin enabled?", file=sys.stderr)
        wf_info["display"].disconnect()
        exit(-1)

    return wf_info

def main():
    signal.signal(signal.SIGINT, signal.SIG_DFL)

    app = QApplication(sys.argv)
    clipboard = app.clipboard()

    wf_info = wf_info_create()
    window = WfInfoApp(wf_info, clipboard)

    window.show()

    ret = app.exec_()
    wf_info["display"].disconnect()
    sys.exit(ret)

if __name__ == "__main__":
    main()
