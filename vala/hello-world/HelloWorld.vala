public class HelloWorld : Gtk.Application {
	public HelloWorld() {
		Object(application_id: "com.example.App");
	}

	public override void activate() {
		var win = new Gtk.ApplicationWindow(this);
		var btn = new Gtk.Button.with_label("Hello World");
		btn.clicked.connect(win.close);
		win.child = btn;
		win.present();
	}

	public static int main(string[] args) {
		return new HelloWorld().run(args);
	}
}