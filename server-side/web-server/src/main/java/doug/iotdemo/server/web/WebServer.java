package doug.iotdemo.server.web;

import io.vertx.core.Vertx;
import io.vertx.ext.web.Router;
import io.vertx.ext.web.handler.LoggerFormat;
import io.vertx.ext.web.handler.LoggerHandler;
import io.vertx.ext.web.handler.StaticHandler;

public class WebServer {

	public static void main(String[] args) {
		try {
			new WebServer().run();
		} catch (Throwable e) {
			e.printStackTrace();
		}
	}

	private void run() {
		Vertx vertx = Vertx.factory.vertx();

		Router router = Router.router(vertx);

		router.route().handler(LoggerHandler.create(LoggerFormat.DEFAULT));

		router.route().handler(StaticHandler.create("www").setCachingEnabled(false));

		vertx.createHttpServer().requestHandler(router::accept).listen(8080);
		System.out.println("Listening on port 8080");
	}

}
