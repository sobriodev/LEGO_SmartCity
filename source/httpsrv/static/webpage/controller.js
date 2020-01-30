const legoApp = angular.module('LegoApp',[]);

legoApp.controller('LegoCtrl', ['$scope', function($scope) {
    $scope.lego = LegoData;

    $scope.imgPath = './lego_img/';
    $scope.imgDir = function (id) {
        return $scope.imgPath + id + '.jpg';
    };

    $scope.toggleLight = function (id) {
        $.ajax({
            type: 'POST',
            url: 'lego-light.api',
            contentType: 'application/json',
            data: JSON.stringify({
                id: id,
                mode: 'single',
                operation: 'toggle'
            }),
            error: function (data) {
                alert('Request processing error (status ' + data.status + ')');
            },
            dataType: 'json'
        });
    };

    $scope.groupStatus = function (ids, state) {
        ids.forEach(function (id) {
            $.ajax({
                type: 'POST',
                url: 'lego-light.api',
                contentType: 'application/json',
                data: JSON.stringify({
                    id: id,
                    mode: 'group',
                    operation: state ? 'on' : 'off'
                }),
                error: function (data) {
                    alert('Request processing error (status ' + data.status + ')');
                },
                dataType: 'json'
            });
        })
    };

    $scope.animData = [false, false, false, false, false];

    $scope.animStatus = function (id) {
        $scope.animData[id] = !$scope.animData[id];

        $.ajax({
            type: 'POST',
            url: 'lego-anim-status.api',
            contentType: 'application/json',
            data: JSON.stringify({
                id: id,
                animStatus: $scope.animData[id] ? 1 : 0,
            }),
            error: function (data) {
                alert('Request processing error (status ' + data.status + ')');
            },
            dataType: 'json'
        });
    }

}]);