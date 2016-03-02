var path = require('path');

module.exports = {
	entry : './app/main.js',
	output : {
		filename : 'bundle.js'
	},
	resolve : {
		root : path.resolve('./app'),
		extensions : [ '', '.js' ]
	},
	devtool : 'source-map',
	module : {
		loaders : [ {
			test : /\.js$/,
			exclude : /node_modules/,
			loaders : [ 'babel-loader?cacheDirectory&presets[]=react&presets[]=es2015' ]
		} ]
	}
};
