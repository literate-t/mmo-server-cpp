
import argparse
import jinja2
import ProtoParser

def main():
    arg_parser = argparse.ArgumentParser(description = "PacketGenerator")
    arg_parser.add_argument("--path", type=str, default="D:/CPP_Server/Protobuf/bin/Protocol.proto" , help="proto path")
    arg_parser.add_argument("--output", type=str, default="TestPacketHandler" , help="output file")
    arg_parser.add_argument("--recv", type=str, default="C_" , help="recv convention")
    arg_parser.add_argument("--send", type=str, default="S_" , help="send convevntion")
    args = arg_parser.parse_args()
    
    parser = ProtoParser.ProtoParser(1000, args.recv, args.send)
    # 수신 패킷, 송신 패킷을 구분하고 패킷 아이디 부여를 위해 모든 패킷의 정보도 갖는다
    parser.parse_proto(args.path)
    
    # jinja2
    # 자동화 작업을 위한 헤더 파일이 위치한 폴더
    file_loader = jinja2.FileSystemLoader("Templates")
    env = jinja2.Environment(loader=file_loader)
    
    #우리가 작업할 헤더 파일
    packet_handler_template = env.get_template("PacketHandler.h")
    # output은 프로그램을 실행할 때 넣을 옵션 값 중에 하나로 클래스의 이름에 쓰인다
    # parser=parser를 통해 내부에서 parser 객체의 멤버 변수를 쓸 수 있다
    result = packet_handler_template.render(parser=parser, output=args.output)
    
    file = open(args.output+".h", "w+");
    file.write(result)
    file.close()
    
    print(result)

    return

if __name__ == "__main__":
    main()
    

