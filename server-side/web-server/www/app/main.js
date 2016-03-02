import React from 'react';
import ReactDOM from 'react-dom';

export class SensorButton extends React.Component {
	render() {
		return (
			<div className="col-sm-3">
				<button type="button" className="btn btn-success sbtn">Sensor {this.props.sensor}</button><br />
			</div>
		);
	}
}

export class Sensors extends React.Component {
	render() {
		var list = [];
		for (var i = 1; i < 21; ++i) {
			list.push(i);
		}

		var buttons = list.map(function(id) {
			return <SensorButton key={id} sensor={id} />
		});
		
		return (
			<div>
				<div className="row">
					{buttons}
				</div>
			</div>
		);
	}
}

ReactDOM.render(
		<div>
			<h3>Doug's IoT Demo</h3>
			<Sensors />
		</div>,
		document.getElementById("main")
);
