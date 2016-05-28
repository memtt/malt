'use strict';

/**
 * @ngdoc overview
 * @name mattWebviewApp
 * @description
 * # mattWebviewApp
 *
 * Main module of the application.
 */
angular
  .module('mattWebviewApp', [
    'ngAnimate',
    'ngCookies',
    'ngResource',
    'ngRoute',
    'ngSanitize',
    'ngTouch',
	'matt.anim',
	'matt.page.home'
  ])
  .config(function ($routeProvider) {
    $routeProvider
      .when('/', {
        templateUrl: 'views/main.html',
        controller: 'matt.page.home.ctrl',
        controllerAs: 'main',
		pageKey: 'home'
      })
      .when('/about', {
        templateUrl: 'views/about.html',
        controller: 'AboutCtrl',
        controllerAs: 'about',
		pageKey: 'about'
      })
      .otherwise({
        redirectTo: '/'
      });
  });
