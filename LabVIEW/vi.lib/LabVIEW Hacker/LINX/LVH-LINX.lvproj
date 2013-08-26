<?xml version='1.0' encoding='UTF-8'?>
<Project Type="Project" LVVersion="11008008">
	<Item Name="My Computer" Type="My Computer">
		<Property Name="server.app.propertiesEnabled" Type="Bool">true</Property>
		<Property Name="server.control.propertiesEnabled" Type="Bool">true</Property>
		<Property Name="server.tcp.enabled" Type="Bool">false</Property>
		<Property Name="server.tcp.port" Type="Int">0</Property>
		<Property Name="server.tcp.serviceName" Type="Str">My Computer/VI Server</Property>
		<Property Name="server.tcp.serviceName.default" Type="Str">My Computer/VI Server</Property>
		<Property Name="server.vi.callsEnabled" Type="Bool">true</Property>
		<Property Name="server.vi.propertiesEnabled" Type="Bool">true</Property>
		<Property Name="specify.custom.address" Type="Bool">false</Property>
		<Item Name="POC" Type="Folder">
			<Item Name="Diagram Disable Auto Enable" Type="Folder">
				<Item Name="DD Enabler.vi" Type="VI" URL="/&lt;vilib&gt;/LabVIEW Hacker/LINX/POC/Diagram Disable Auto Enable/DD Enabler.vi"/>
				<Item Name="DD Test Subject.vi" Type="VI" URL="/&lt;vilib&gt;/LabVIEW Hacker/LINX/POC/Diagram Disable Auto Enable/DD Test Subject.vi"/>
			</Item>
			<Item Name="Serial and TCP Benchmarking" Type="Folder">
				<Item Name="Results" Type="Folder">
					<Item Name="Arduino Uno - Serial.PNG" Type="Document" URL="/&lt;vilib&gt;/LabVIEW Hacker/LINX/POC/Serial and TCP Benchmarking/Results/Arduino Uno - Serial.PNG"/>
					<Item Name="chipKIT - Ethernet Zoomed.PNG" Type="Document" URL="/&lt;vilib&gt;/LabVIEW Hacker/LINX/POC/Serial and TCP Benchmarking/Results/chipKIT - Ethernet Zoomed.PNG"/>
					<Item Name="chipKIT - Ethernet.PNG" Type="Document" URL="/&lt;vilib&gt;/LabVIEW Hacker/LINX/POC/Serial and TCP Benchmarking/Results/chipKIT - Ethernet.PNG"/>
					<Item Name="chipKIT - Serial.PNG" Type="Document" URL="/&lt;vilib&gt;/LabVIEW Hacker/LINX/POC/Serial and TCP Benchmarking/Results/chipKIT - Serial.PNG"/>
					<Item Name="chipKIT - WIFI.PNG" Type="Document" URL="/&lt;vilib&gt;/LabVIEW Hacker/LINX/POC/Serial and TCP Benchmarking/Results/chipKIT - WIFI.PNG"/>
				</Item>
				<Item Name="Serial" Type="Folder">
					<Item Name="LINX_Arduino_Serial_Benchmark" Type="Folder">
						<Item Name="LINX_Arduino_Serial_Benchmark.ino" Type="Document" URL="/&lt;vilib&gt;/LabVIEW Hacker/LINX/POC/Serial and TCP Benchmarking/Serial/LINX_Arduino_Serial_Benchmark/LINX_Arduino_Serial_Benchmark.ino"/>
					</Item>
					<Item Name="LINX_chipKIT_Serial_Benchmark" Type="Folder">
						<Item Name="LINX_chipKIT_Serial_Benchmark.pde" Type="Document" URL="/&lt;vilib&gt;/LabVIEW Hacker/LINX/POC/Serial and TCP Benchmarking/Serial/LINX_chipKIT_Serial_Benchmark/LINX_chipKIT_Serial_Benchmark.pde"/>
					</Item>
					<Item Name="LINX Serial Benchmark.vi" Type="VI" URL="/&lt;vilib&gt;/LabVIEW Hacker/LINX/POC/Serial and TCP Benchmarking/Serial/LINX Serial Benchmark.vi"/>
				</Item>
				<Item Name="TCP" Type="Folder">
					<Item Name="LINX_chipKIT_Ethernet_Benchmark" Type="Folder">
						<Item Name="LINX_chipKIT_Ethernet_Benchmark.pde" Type="Document" URL="/&lt;vilib&gt;/LabVIEW Hacker/LINX/POC/Serial and TCP Benchmarking/TCP/LINX_chipKIT_Ethernet_Benchmark/LINX_chipKIT_Ethernet_Benchmark.pde"/>
					</Item>
					<Item Name="LINX_chipKIT_WIFI_Benchmark" Type="Folder">
						<Item Name="LINX_chipKIT_WIFI_Benchmark.pde" Type="Document" URL="/&lt;vilib&gt;/LabVIEW Hacker/LINX/POC/Serial and TCP Benchmarking/TCP/LINX_chipKIT_WIFI_Benchmark/LINX_chipKIT_WIFI_Benchmark.pde"/>
					</Item>
					<Item Name="LINX TCP Benchmark.vi" Type="VI" URL="/&lt;vilib&gt;/LabVIEW Hacker/LINX/POC/Serial and TCP Benchmarking/TCP/LINX TCP Benchmark.vi"/>
				</Item>
			</Item>
		</Item>
		<Item Name="Test Apps" Type="Folder">
			<Item Name="Device Connection Test App.vi" Type="VI" URL="/&lt;vilib&gt;/LabVIEW Hacker/LINX/Test Apps/Device Connection Test App.vi"/>
			<Item Name="LINX Connection Test App.vi" Type="VI" URL="/&lt;vilib&gt;/LabVIEW Hacker/LINX/Test Apps/LINX Connection Test App.vi"/>
		</Item>
		<Item Name="LVH-LINX.lvlib" Type="Library" URL="../LVH-LINX.lvlib"/>
		<Item Name="Dependencies" Type="Dependencies">
			<Item Name="vi.lib" Type="Folder">
				<Item Name="Error Cluster From Error Code.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Error Cluster From Error Code.vi"/>
				<Item Name="LVH-LINX.lvlib" Type="Library" URL="/&lt;vilib&gt;/LabVIEW Hacker/LINX/LVH-LINX.lvlib"/>
				<Item Name="NI_PtbyPt.lvlib" Type="Library" URL="/&lt;vilib&gt;/ptbypt/NI_PtbyPt.lvlib"/>
				<Item Name="TRef Traverse.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/traverseref.llb/TRef Traverse.vi"/>
				<Item Name="TRef TravTarget.ctl" Type="VI" URL="/&lt;vilib&gt;/Utility/traverseref.llb/TRef TravTarget.ctl"/>
				<Item Name="Trim Whitespace.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Trim Whitespace.vi"/>
				<Item Name="VI Scripting - Traverse.lvlib" Type="Library" URL="/&lt;vilib&gt;/Utility/traverseref.llb/VI Scripting - Traverse.lvlib"/>
				<Item Name="VISA Configure Serial Port" Type="VI" URL="/&lt;vilib&gt;/Instr/_visa.llb/VISA Configure Serial Port"/>
				<Item Name="VISA Configure Serial Port (Instr).vi" Type="VI" URL="/&lt;vilib&gt;/Instr/_visa.llb/VISA Configure Serial Port (Instr).vi"/>
				<Item Name="VISA Configure Serial Port (Serial Instr).vi" Type="VI" URL="/&lt;vilib&gt;/Instr/_visa.llb/VISA Configure Serial Port (Serial Instr).vi"/>
				<Item Name="whitespace.ctl" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/whitespace.ctl"/>
			</Item>
		</Item>
		<Item Name="Build Specifications" Type="Build"/>
	</Item>
</Project>
