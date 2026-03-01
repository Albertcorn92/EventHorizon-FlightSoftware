module GncDeployment {

    active component ProximityDriver {

        # Hardware Connection Ports
        # Matching Drv.ByteStreamData exactly to satisfy the topology connection
        async input port serialRecv: Drv.ByteStreamData

        sync input port ready: Drv.ByteStreamReady

        output port deallocate: Fw.BufferSend

        # Sensor Telemetry Channels
        telemetry Z_Distance:      F32
        telemetry Signal_Strength: F32
        telemetry X_Error:         F32
        telemetry Y_Error:         F32

        # Standard F Prime Boilerplate
        time get port timeCaller

        command recv port cmdIn
        command reg port cmdRegOut
        command resp port cmdResponseOut

        event port logOut
        text event port logTextOut

        telemetry port tlmOut

        param get port prmGetOut
        param set port prmSetOut

    }

}