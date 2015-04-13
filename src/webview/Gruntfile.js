module.exports = function(grunt) {
	
	var jsFiles = [
		"client-files/app/bower_components/jquery/dist/jquery.min.js",
		"client-files/app/bower_components/jquery-treetable/jquery.treetable.js",
		"client-files/app/bower_components/bootstrap/dist/js/bootstrap.min.js",
		"client-files/app/bower_components/angular/angular.js",
		"client-files/app/bower_components/angular-route/angular-route.js",
		"client-files/app/bower_components/angular-animate/angular-animate.js",
		"client-files/app/bower_components/codemirror/lib/codemirror.js",
		"client-files/app/bower_components/codemirror/mode/clike/clike.js",
		"client-files/bower_components/angular-ui-codemirror/ui-codemirror.js",
		"client-files/bower_components/angular-ui-bootstrap-bower/ui-bootstrap.min.js",
		"client-files/bower_components/angular-ui-bootstrap-bower/ui-bootstrap-tpls.min.js",
		"client-files/app/bower_components/d3/d3.min.js",
		"client-files/app/bower_components/nvd3/nv.d3.min.js",
		"client-files/app/bower_components/d3-tip/d3.tip.min.js",
		"client-files/bower_components/file-saver/FileSaver.min.js",
		//logic
		"client-files/app/js/helper.js",
		"client-files/app/js/app.js",
		"client-files/app/js/controlers.js",
		"client-files/app/js/animations.js",
		//objs
		"client-files/app/js/func-metrics.js",
		"client-files/app/js/source-editor.js",
		"client-files/app/js/call-stack-view.js",
		//pages
// 		"client-files/app/js/MaltBundle.js",
// 		"build/MaltBundle.js",
		//"MaltBundle.js",
		"client-files/app/js/data-source.js",
		"client-files/app/js/page-home.js",
		"client-files/app/js/page-global-vars.js",
		"client-files/app/js/page-alloc-size-distr.js",
		"client-files/app/js/page-realloc.js",
		"client-files/app/js/page-stack-peaks.js",
		"client-files/app/js/page-timeline.js",
		"client-files/app/js/page-sources.js",
		"client-files/app/js/page-per-thread.js",
		//partials
		"build/app-partials.js"
	];
	var cssFiles = [
		"client-files/app/bower_components/jquery-treetable/css/jquery.treetable.css",
		"client-files/app/bower_components/jquery-treetable/css/jquery.treetable.theme.default.css",
		"client-files/app/bower_components/bootstrap/dist/css/bootstrap.min.css",
		"client-files/app/bower_components/bootstrap/dist/css/bootstrap-theme.min.css",
		"client-files/app/bower_components/codemirror/lib/codemirror.css",
		"client-files/app/bower_components/codemirror/theme/eclipse.css",
		"client-files/app/bower_components/nvd3/nv.d3.min.css",
		"client-files/app/css/app.css",
		"client-files/app/css/animations.css"
	];

	// Project configuration.
	grunt.initConfig({
		pkg: grunt.file.readJSON('package.json'),
		concat: {
			options: {
				banner: '/*! <%= pkg.name %> <%= grunt.template.today("yyyy-mm-dd") %> */\n',
// 				separator: ';'
				mangle: false
			},
			dist: {
				files: {
					'dist/<%= pkg.name %>.min.js':jsFiles
				}
			}
		},
		browserify: {
			dist: {
				files: {
					'dist/MaltBundle.js': ['server-files/MaltProject.js'],
				},
				options: {
					browserifyOptions: {
						standalone: "MaltProject",
					}
				}
			}
		},
		cssmin: {
			options: {
				shorthandCompacting: false,
				roundingPrecision: -1
			},
			dist: {
				files: {
					'dist/<%= pkg.name %>.min.css': cssFiles
				}
			}
		},
		copy: {
			main: {
				files: [
					{expand: true,flatten:true, src: ['client-files/app/index-dist.html'], dest: 'dist/', filter: 'isFile'},
					//{expand: true,flatten:true, src: jsFiles, dest: 'dist/js', filter: 'isFile'},
					//{expand: true,flatten:true, src: ['client-files/app/partials/*.html'], dest: 'partials/', filter: 'isFile'},
				],
			},
		},
		ngtemplates:  {
			"malt.app":        {
				cwd:      'client-files/app',
				src:      'partials/**.html',
				dest:     'build/app-partials.js'
			}
		}
	});

	// Load the plugin that provides the "uglify" task.
	grunt.loadNpmTasks('grunt-contrib-concat');
	grunt.loadNpmTasks('grunt-browserify');
	grunt.loadNpmTasks('grunt-contrib-cssmin');
	grunt.loadNpmTasks('grunt-contrib-copy');
	grunt.loadNpmTasks('grunt-angular-templates');

	// Default task(s).
	grunt.registerTask('default', ['browserify','ngtemplates','concat','cssmin','copy']);
};
