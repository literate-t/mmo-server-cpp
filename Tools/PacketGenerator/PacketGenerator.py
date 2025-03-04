
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
    # ���� ��Ŷ, �۽� ��Ŷ�� �����ϰ� ��Ŷ ���̵� �ο��� ���� ��� ��Ŷ�� ������ ���´�
    parser.parse_proto(args.path)
    
    # jinja2
    # �ڵ�ȭ �۾��� ���� ��� ������ ��ġ�� ����
    file_loader = jinja2.FileSystemLoader("Templates")
    env = jinja2.Environment(loader=file_loader)
    
    #�츮�� �۾��� ��� ����
    packet_handler_template = env.get_template("PacketHandler.h")
    # output�� ���α׷��� ������ �� ���� �ɼ� �� �߿� �ϳ��� Ŭ������ �̸��� ���δ�
    # parser=parser�� ���� ���ο��� parser ��ü�� ��� ������ �� �� �ִ�
    result = packet_handler_template.render(parser=parser, output=args.output)
    
    file = open(args.output+".h", "w+");
    file.write(result)
    file.close()
    
    print(result)

    return

if __name__ == "__main__":
    main()
    

