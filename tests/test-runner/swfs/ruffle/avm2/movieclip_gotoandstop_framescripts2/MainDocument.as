﻿package {
	import flash.display.MovieClip;

	public class MainDocument extends MovieClip {
		public function MainDocument() {
			trace("/// this.child.gotoAndStop(2)");
			this.child.gotoAndStop(2);

			trace("/// this.stop()");

			this.stop();
		}
	}
}