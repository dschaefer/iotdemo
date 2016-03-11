import React from 'react';
import ReactDOM from 'react-dom';

export class SensorButton extends React.Component {
	onMouseEvent(state) {
		var sensor = 'sensor' + this.props.sensor;
		$.ajax({
			url: this.props.url + "?sensor=" + sensor + "&state=" + state,
			method: 'PUT'
		});
	}
	
	onMouseDown() {
		this.onMouseEvent(1);
	}

	onMouseUp() {
		this.onMouseEvent(0);
	}
	
	render() {
		var color = "btn btn-success sbtn";
		var thresh = 20000;
		if (this.props.data) {
			if (this.props.data.state == 1) {
				color = "btn btn-warning sbtn";
			} else if (this.props.data.state == 2) {
				color = "btn btn-danger sbtn";
			}
			thresh = this.props.data.thresh;
		}
		
		return (
			<div className="col-sm-3">
				<button type="button" className={color} onMouseDown={this.onMouseDown.bind(this)} onMouseUp={this.onMouseUp.bind(this)}>
					Sensor {this.props.sensor} ({thresh})
				</button>
			</div>
		);
	}
}

export class Sensors extends React.Component {
	constructor() {
		super();
		this.state = { data: {} };

		this.list = [];
		for (var i = 1; i < 21; ++i) {
			this.list.push(i);
		}
	}

	fetchFromServer() {
		var sensors = this;
		$.ajax({
			url: this.props.url,
			dataType: 'json',
			success: (data) => {
				sensors.setState({ data: data });
			},
			error: (xhr, status, err) => {
				console.error('state fetch', status, err.toString());
			}
		});
	}

	componentDidMount() {
		this.fetchFromServer();
		setInterval(this.fetchFromServer.bind(this), 1000);
	}

	render() {
		var buttons = this.list.map((id) => {
			return <SensorButton key={id} sensor={id} data={this.state.data["sensor" + id]} url={this.props.url}/>
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
			<Sensors url='http://localhost:8080/api/sensors'/>
		</div>,
		document.getElementById("main")
);
